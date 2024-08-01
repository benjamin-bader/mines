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
#include <QSettings>
#include <QVariant>

#include <algorithm>
#include <random>
#include <ranges>

namespace {

constexpr const int kCellSize = 30;

constexpr const GameBoard kSmallGame{10, 10, 15};
constexpr const GameBoard kMediumGame{15, 15, 45};
constexpr const GameBoard kLargeGame{16, 30, 99};

auto allCells(const GameBoard& board)
{
    using namespace std::ranges::views;

    int rows = board.rows();
    int cols = board.cols();
    int numCells = rows * cols;

    auto toPoint = [cols](int n) {
        int x = n % cols;
        int y = n / cols;
        return QPoint{x, y};
    };

    return iota(0, numCells) | transform(toPoint);
}

auto neighborsOf(const GameBoard& board, QPoint point)
{
    using namespace std::ranges::views;

    static constexpr QPoint offsets[] {
        {-1, -1}, {0, -1}, {1, -1},
        {-1,  0},          {1,  0},
        {-1,  1}, {0,  1}, {1,  1},
    };

    QRect bounds(0, 0, board.cols(), board.rows());

    auto ns = offsets
           | transform([point](QPoint o) { return point + o; })
           | filter([bounds](QPoint p) { return bounds.contains(p); });

    return ns;
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QSettings settings;
    QVariant lastSize = settings.value("board");
    GameBoard size = kMediumGame;
    if (lastSize.isValid())
    {
        size = lastSize.value<GameBoard>();
    }

    setWindowTitle(tr("Mines"));
    initializeActions();
    initializeMenu();

    initializeGame(size);
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
        for (const auto& neighbor : neighborsOf(m_board, coords))
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

void MainWindow::initializeGame(GameBoard board)
{
    m_board = board;
    initializeGrid();

    setFixedSize(kCellSize * cols(), kCellSize * rows());

    if (board == kSmallGame)
    {
        m_smallGame->setChecked(true);
    }
    else if (board == kMediumGame)
    {
        m_mediumGame->setChecked(true);
    }
    else if (board == kLargeGame)
    {
        m_largeGame->setChecked(true);
    }

    QSettings settings;
    settings.setValue("board", QVariant::fromValue(board));
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
    m_cells.reserve(rows() * cols());
    for (QPoint coord : allCells(m_board))
    {
        Cell* cell = new Cell(coord, widget);
        cell->setSizePolicy(sizePolicy);
        cell->setMinimumHeight(kCellSize);
        cell->setMinimumWidth(kCellSize);

        connect(cell, &Cell::revealed, this, &MainWindow::cellRevealed);
        connect(this, &MainWindow::gameEnded, cell, &Cell::gameEnded);

        grid->addWidget(cell, coord.y(), coord.x());
        m_cells << cell;
    }
    m_cells.squeeze(); // if we've gone from a large to a small game, m_cells is over-allocated.  release.

    // Next, mark cells that are mines.
    int numMines = mines();

    QList<int> indices(rows() * cols());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), std::mt19937(std::random_device{}()));

    auto toPoint = [&](int n) { return QPoint{n % cols(), n / cols()}; };
    auto nonCorner = [&](QPoint p) { return !isCorner(p); };
    auto mineCells = indices
                     | std::ranges::views::transform(toPoint)
                     | std::ranges::views::filter(nonCorner)
                     | std::ranges::views::take(numMines);

    for (QPoint point : mineCells)
    {
        cellAt(point)->setNumNeighboringMines(-1);
    }

    // Finally, mark all non-mines with the count of surrounding mines.
    for (QPoint coord : allCells(m_board))
    {
        Cell* cell = cellAt(coord);
        if (cell->isMine())
        {
            continue;
        }

        using namespace std::ranges::views;

        auto ns = neighborsOf(m_board, coord)
                  | transform([&](auto p) { return cellAt(p); })
                  | filter([](auto c) { return c->isMine(); });

        auto numNeighbors = std::ranges::distance(ns);

        cell->setNumNeighboringMines(numNeighbors);
    }
}

Cell* MainWindow::cellAt(QPoint coord) const
{
    return m_cells[(coord.y() * cols()) + coord.x()];
}

bool MainWindow::isBoardSolved() const
{
    return std::all_of(
        m_cells.begin(),
        m_cells.end(),
        [](Cell* c) { return c->isRevealed() || c->isMine(); });
}

bool MainWindow::isCorner(const QPoint& point) const
{
    int xmin = 0;
    int ymin = 0;
    int xmax = rows() - 1;
    int ymax = cols() - 1;
    return (point.x() == xmin && point.y() == ymin)
           || (point.x() == xmin && point.y() == ymax)
           || (point.x() == xmax && point.y() == ymin)
           || (point.x() == xmax && point.y() == ymax);
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
