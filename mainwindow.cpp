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

#include <QGridLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

#include <algorithm>
#include <random>
#include <ranges>

namespace {

constexpr const int kCellSize = 30;

struct GameSize
{
    int rows;
    int cols;
    int mines;
};

constexpr const GameSize kSmallGame = { .rows = 10, .cols = 10, .mines = 15 };
constexpr const GameSize kMediumGame = { .rows = 15, .cols = 15, .mines = 45 };
constexpr const GameSize kLargeGame = { .rows = 16, .cols = 30, .mines = 99 };

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Mines"));
    initializeActions();
    initializeMenu();

    initializeGame(kMediumGame.rows, kMediumGame.cols, kMediumGame.mines);
}

MainWindow::~MainWindow() {}

void MainWindow::cellRevealed(QPoint coords)
{
    Cell* cell = cellAt(coords);
    if (cell->isMine())
    {
        lose();
        return;
    }

    if (cell->getNumNeighboringMines() == 0)
    {
        for (const auto& neighbor : neighborsOf(coords))
        {
            cellAt(neighbor)->reveal();
        }

        return;
    }

    if (isBoardSolved())
    {
        win();
    }
}

void MainWindow::initializeActions()
{
    m_smallGame = new QAction;
    m_smallGame->setText(tr("Small"));
    connect(m_smallGame, &QAction::triggered, this, [&]() { initializeGame(kSmallGame.rows, kSmallGame.cols, kSmallGame.mines); });

    m_mediumGame = new QAction;
    m_mediumGame->setText(tr("Medium"));
    connect(m_mediumGame, &QAction::triggered, this, [&]() { initializeGame(kMediumGame.rows, kMediumGame.cols, kMediumGame.mines); });

    m_largeGame = new QAction;
    m_largeGame->setText(tr("Large"));
    connect(m_largeGame, &QAction::triggered, this, [&]() { initializeGame(kLargeGame.rows, kLargeGame.cols, kLargeGame.mines); });

    m_gameSizeGroup = new QActionGroup(this);
    m_gameSizeGroup->addAction(m_smallGame);
    m_gameSizeGroup->addAction(m_mediumGame);
    m_gameSizeGroup->addAction(m_largeGame);

    m_mediumGame->setChecked(true);
}

void MainWindow::initializeMenu()
{
    QMenu* file = menuBar()->addMenu(tr("&File"));

    QAction* about = file->addAction(tr("&About"));
    about->setMenuRole(QAction::AboutRole);
    about->setStatusTip(tr("About this program"));
    // TODO

    file->addSeparator()->setText(tr("Game Size"));

    file->addAction(m_smallGame);
    file->addAction(m_mediumGame);
    file->addAction(m_largeGame);

    file->addSeparator();

    QAction* quit = file->addAction(tr("&Quit"));
    quit->setMenuRole(QAction::QuitRole);
    quit->setShortcut(QKeySequence::Quit);
    quit->setStatusTip(tr("Quit"));
    connect(quit, &QAction::triggered, &QCoreApplication::quit);
}

void MainWindow::initializeGame(int numRows, int numCols, int numMines)
{
    m_numRows = numRows;
    m_numCols = numCols;
    m_numMines = numMines;
    initializeGrid();

    setFixedSize(kCellSize * m_numCols, kCellSize * m_numRows);
}

void MainWindow::initializeGrid()
{
    if (centralWidget() != nullptr)
    {
        centralWidget()->deleteLater();
    }

    QGridLayout* grid = new QGridLayout;
    grid->setHorizontalSpacing(0);
    grid->setVerticalSpacing(0);
    grid->setContentsMargins(0, 0, 0, 0);

    QWidget* widget = new QWidget;
    widget->setLayout(grid);
    setCentralWidget(widget);

    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
    sizePolicy.setWidthForHeight(true);

    m_cells.clear();
    m_cells.reserve(m_numRows * m_numCols);
    for (int y = 0; y < m_numRows; ++y)
    {
        for (int x = 0; x < m_numCols; ++x)
        {
            QPoint coord(x, y);
            Cell* cell = new Cell(coord, widget);
            cell->setSizePolicy(sizePolicy);
            cell->setMinimumHeight(kCellSize);
            cell->setMinimumWidth(kCellSize);

            connect(cell, &Cell::revealed, this, &MainWindow::cellRevealed);
            connect(this, &MainWindow::gameEnded, cell, &Cell::gameEnded);

            grid->addWidget(cell, y, x);
            m_cells << cell;
        }
    }
    m_cells.squeeze(); // if we've gone from a large to a small game, m_cells is over-allocated.  release.

    int numMines = m_numMines;

    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> rowDistrib(0, m_numRows - 1);
    std::uniform_int_distribution<> colDistrib(0, m_numCols - 1);

    while (numMines > 0)
    {
        int row = rowDistrib(gen);
        int col = colDistrib(gen);

        if ((row == 0 && col == 0)
            || (row == m_numRows - 1 && col == 0)
            || (row == 0 && col == m_numCols - 1)
            || (row == m_numRows -1 && col == m_numCols - 1))
        {
            // no corners!
            continue;
        }

        Cell* cell = cellAt(QPoint(col, row));

        if (cell->isMine())
        {
            continue;
        }

        cell->setNumNeighboringMines(-1);
        numMines--;
    }

    for (int y = 0; y < m_numRows; ++y)
    {
        for (int x = 0; x < m_numCols; ++x)
        {
            QPoint coord(x, y);

            if (cellAt(coord)->isMine())
            {
                continue;
            }

            int numNeighbors = 0;
            for (const auto& neighbor : neighborsOf(coord))
            {
                Cell* cell = cellAt(neighbor);
                if (cell->isMine())
                {
                    numNeighbors++;
                }
            }

            cellAt(coord)->setNumNeighboringMines(numNeighbors);
        }
    }
}

Cell* MainWindow::cellAt(QPoint coord)
{
    return m_cells[(coord.y() * m_numCols) + coord.x()];
}

QList<QPoint> MainWindow::neighborsOf(QPoint coord) const
{
    static QList<QPoint> offsets{
        QPoint(-1, -1), QPoint(0, -1), QPoint(1, -1),
        QPoint(-1, 0), /* QPoint(0, 0), */ QPoint(1, 0),
        QPoint(-1, 1), QPoint(0, 1), QPoint(1, 1),
    };

    QRect bounds(QPoint(0, 0), QPoint(m_numCols - 1, m_numRows - 1));

    auto neighbors = offsets
        | std::views::transform([&](auto offset) { return coord + offset; })
        | std::views::filter([&](auto n) { return bounds.contains(n); });

    return {std::begin(neighbors), std::end(neighbors)};
}

bool MainWindow::isBoardSolved() const
{
    return std::all_of(
        m_cells.begin(),
        m_cells.end(),
        [](Cell* c) { return c->isRevealed() || c->isMine(); });
}

void MainWindow::win()
{
    if (!centralWidget()->isEnabled())
    {
        return;
    }

    centralWidget()->setEnabled(false);

    emit gameEnded();

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
    if (!centralWidget()->isEnabled())
    {
        return;
    }

    centralWidget()->setEnabled(false);

    emit gameEnded();

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
