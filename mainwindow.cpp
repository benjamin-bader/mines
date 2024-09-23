// Mines
//
// Copyright (C) 2024 Benjamin Bader
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "mainwindow.h"

#include "aboutdialog.h"
#include "customgamedialog.h"
#include "minefield.h"

#include <QDebug>
#include <QGridLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QVariant>

#include <QtGlobal>

namespace {

constexpr const int kCellSize = 30;

constexpr const GameBoard kSmallGame{10, 10, 15};
constexpr const GameBoard kMediumGame{15, 15, 45};
constexpr const GameBoard kLargeGame{16, 30, 99};

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_about{nullptr}
    , m_clock{new Clock(this)}
{
    QSettings settings;
    GameBoard size;
    size.load(settings);

    setWindowTitle(tr("Mines"));
    initializeActions();
    initializeMenu();

    connect(m_clock, &Clock::tick, this, &MainWindow::clockTicked);

    initializeGame(size);
}

void MainWindow::showAboutDialog()
{
    if (m_about == nullptr)
    {
        m_about = new AboutDialog{this};
    }

    m_about->show();
    m_about->raise();
    m_about->activateWindow();
}

void MainWindow::initializeActions()
{
    m_smallGame = new QAction;
    m_smallGame->setText(tr("Small"));
    m_smallGame->setCheckable(true);
    connect(m_smallGame, &QAction::triggered, this, [&]() { initializeGame(kSmallGame); });

    m_mediumGame = new QAction;
    m_mediumGame->setText(tr("Medium"));
    m_mediumGame->setCheckable(true);
    connect(m_mediumGame, &QAction::triggered, this, [&]() { initializeGame(kMediumGame); });

    m_largeGame = new QAction;
    m_largeGame->setText(tr("Large"));
    m_largeGame->setCheckable(true);
    connect(m_largeGame, &QAction::triggered, this, [&]() { initializeGame(kLargeGame); });

    m_customGame = new QAction;
    m_customGame->setText(tr("Custom"));
    m_customGame->setCheckable(true);
    connect(m_customGame, &QAction::triggered, this, &MainWindow::beginCustomGame);

    m_gameSizeGroup = new QActionGroup(this);
    m_gameSizeGroup->addAction(m_smallGame);
    m_gameSizeGroup->addAction(m_mediumGame);
    m_gameSizeGroup->addAction(m_largeGame);
    m_gameSizeGroup->addAction(m_customGame);
}

void MainWindow::initializeMenu()
{
    QMenu* file = menuBar()->addMenu(tr("&File"));

    QAction* about = file->addAction(tr("&About"));
    about->setMenuRole(QAction::AboutRole);
    about->setStatusTip(tr("About this program"));
    connect(about, &QAction::triggered, this, &MainWindow::showAboutDialog);

    file->addSeparator()->setText(tr("Game Size"));

    file->addAction(m_smallGame);
    file->addAction(m_mediumGame);
    file->addAction(m_largeGame);
    file->addAction(m_customGame);

    file->addSeparator();

    QAction* quit = file->addAction(tr("&Quit"));
    quit->setMenuRole(QAction::QuitRole);
    quit->setShortcut(QKeySequence::Quit);
    quit->setStatusTip(tr("Quit"));
    connect(quit, &QAction::triggered, &QCoreApplication::quit);
}

void MainWindow::beginCustomGame(bool checked)
{
    CustomGameDialog dialog{m_board, this};

    if (dialog.exec() == QDialog::Accepted)
    {
        initializeGame(dialog.getBoard());
    }
    else
    {
        // "Custom" will have been checked, but if the dialog was canceled
        // then we want to restore the previously-checked action's checked state.
        updateMenuCheckboxes();
    }
}

void MainWindow::initializeGame(GameBoard board)
{
    m_board = board;
    initializeGrid();

    int fixedWidth = kCellSize * cols();
    int fixedHeight = kCellSize * rows();

#ifndef Q_OS_MACOS
    // macOS uses "global" menu bars at the top of the screen;
    // other platforms have menubars attached at the top of each
    // window.  If we're not on macOS, we need to account for the
    // height of the menu bar or else cells end up squished.
    fixedHeight += menuWidget()->height();
#endif

    setFixedSize(fixedWidth, fixedHeight);

    updateMenuCheckboxes();

    QSettings settings;
    board.save(settings);
}

void MainWindow::updateMenuCheckboxes()
{
    if (m_board == kSmallGame)
    {
        m_smallGame->setChecked(true);
    }
    else if (m_board == kMediumGame)
    {
        m_mediumGame->setChecked(true);
    }
    else if (m_board == kLargeGame)
    {
        m_largeGame->setChecked(true);
    }
    else
    {
        m_customGame->setChecked(true);
    }
}

void MainWindow::initializeGrid()
{
    if (centralWidget() != nullptr)
    {
        centralWidget()->deleteLater();
        setCentralWidget(nullptr);
    }

    m_clock->reset();

    MineField* field = new MineField(m_board, this);

    connect(field, &MineField::gameWon, this, &MainWindow::win);
    connect(field, &MineField::gameLost, this, &MainWindow::lose);

    connect(field, &MineField::gameStarted, m_clock, &Clock::resume);
    connect(field, &MineField::gameWon, m_clock, &Clock::pause);
    connect(field, &MineField::gameLost, m_clock, &Clock::pause);

    setCentralWidget(field);
}

int MainWindow::rows() const
{
    return m_board.rows();
}

int MainWindow::cols() const
{
    return m_board.cols();
}

int MainWindow::mines() const
{
    return m_board.mines();
}

void MainWindow::win()
{
    int ret = QMessageBox::information(
        this,
        tr("You win!"),
        tr("Nice work!  Would you like to try again?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes
    );

    if (ret == QMessageBox::Yes)
    {
        initializeGrid();
    }
}

void MainWindow::lose()
{
    int ret = QMessageBox::critical(
        this,
        tr("You lost"),
        tr("Better luck next time.  Would you like to try again?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes
    );

    if (ret == QMessageBox::Yes)
    {
        initializeGrid();
    }
}

void MainWindow::clockTicked(int elapsed)
{
    qInfo() << "Tick: " << elapsed;
}
