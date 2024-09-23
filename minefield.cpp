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

#include "minefield.h"

#include <QGridLayout>
#include <QSizePolicy>

#include <algorithm>
#include <random>
#include <ranges>
#include <vector>

namespace {

constexpr int kCellSize = 30;

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

    QRect bounds{0, 0, board.cols(), board.rows()};

    auto applyOffset = [point](QPoint o) { return point + o; };
    auto withinBounds = [bounds = std::move(bounds)](QPoint p) { return bounds.contains(p); };

    return offsets
           | transform(applyOffset)
           | filter(withinBounds);
}

}

MineField::MineField(GameBoard board, QWidget *parent)
    : QWidget{parent}
    , m_board{board}
    , m_cells{}
    , m_started{}
{
    QGridLayout* grid = new QGridLayout;
    grid->setHorizontalSpacing(0);
    grid->setVerticalSpacing(0);
    grid->setContentsMargins(0, 0, 0, 0);

    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
    sizePolicy.setWidthForHeight(true);

    m_cells.reserve(rows() * cols());
    for (QPoint coord : allCells(m_board))
    {
        Cell* cell = new Cell(coord, this);
        cell->setSizePolicy(sizePolicy);
        cell->setMinimumHeight(kCellSize);
        cell->setMinimumWidth(kCellSize);

        connect(cell, &Cell::revealed, this, &MineField::cellRevealed);
        connect(this, &MineField::gameWon, cell, &Cell::gameEnded);
        connect(this, &MineField::gameLost, cell, &Cell::gameEnded);

        grid->addWidget(cell, coord.y(), coord.x());
        m_cells << cell;
    }

    // Next, mark cells that are mines.
    // Make a random list of cell indices, take as many non-corner cells as required,
    // and mark them as mines.

    std::vector<int> indices(rows() * cols()); // I'd use a QList here but MSVC complains about implicit conversion between iterators and pointers
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), std::mt19937(std::random_device{}()));

    auto toPoint = [cols = cols()](int n) { return QPoint{n % cols, n / cols}; };
    auto nonCorner = [&](QPoint p) { return !isCorner(p); };
    auto mineCells = indices
                     | std::ranges::views::transform(toPoint)
                     | std::ranges::views::filter(nonCorner)
                     | std::ranges::views::take(m_board.mines());

    for (QPoint point : mineCells)
    {
        Q_ASSERT(!isCorner(point));
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

    setLayout(grid);
}

void MineField::cellRevealed(const QPoint& coord)
{
    if (!m_started)
    {
        m_started = true;
        emit gameStarted();
    }

    Cell* cell = cellAt(coord);
    if (cell->isMine())
    {
        lose();
        return;
    }

    if (cell->getNumNeighboringMines() == 0)
    {
        for (const auto& neighbor : neighborsOf(m_board, coord))
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

bool MineField::isBoardSolved() const
{
    return std::all_of(
        std::begin(m_cells),
        std::end(m_cells),
        [](Cell* c) { return c->isRevealed() || c->isMine(); }
    );
}

bool MineField::isCorner(const QPoint& point) const
{
    int xmin = 0;
    int ymin = 0;
    int xmax = cols() - 1;
    int ymax = rows() - 1;
    return (point.x() == xmin && point.y() == ymin)
           || (point.x() == xmax && point.y() == ymin)
           || (point.x() == xmin && point.y() == ymax)
           || (point.x() == xmax && point.y() == ymax);
}

int MineField::rows() const
{
    return m_board.rows();
}

int MineField::cols() const
{
    return m_board.cols();
}

Cell* MineField::cellAt(const QPoint& coord)
{
    int ix = coord.y() * cols() + coord.x();
    return m_cells[ix];
}

void MineField::win()
{
    if (!isEnabled())
    {
        return;
    }

    setEnabled(false);

    emit gameWon();
}

void MineField::lose()
{
    if (!isEnabled())
    {
        return;
    }

    setEnabled(false);

    emit gameLost();
}
