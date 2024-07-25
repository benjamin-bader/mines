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
#include <QMessageBox>

#include <random>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_numRows(15)
    , m_numCols(15)
    , m_grid(nullptr)
{
    initializeMenu();
    initializeGrid();

    int cellSize = 30;

    setFixedSize(cellSize * m_numRows, cellSize * m_numCols);
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
        // recurse through neighbors
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

void MainWindow::initializeMenu()
{

}

void MainWindow::initializeGrid()
{
    if (m_grid != nullptr)
    {
        m_grid->deleteLater();
    }

    if (centralWidget() != nullptr)
    {
        centralWidget()->deleteLater();
    }

    m_grid = new QGridLayout;
    m_grid->setHorizontalSpacing(0);
    m_grid->setVerticalSpacing(0);
    m_grid->setContentsMargins(0, 0, 0, 0);

    QWidget* widget = new QWidget;
    widget->setLayout(m_grid);
    setCentralWidget(widget);

    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
    sizePolicy.setWidthForHeight(true);

    m_cells.clear();
    for (int y = 0; y < m_numRows; ++y)
    {
        for (int x = 0; x < m_numCols; ++x)
        {
            QPoint coord(x, y);
            Cell* cell = new Cell(coord, this);
            cell->setSizePolicy(sizePolicy);

            connect(cell, &Cell::revealed, this, &MainWindow::cellRevealed);
            connect(this, &MainWindow::gameEnded, cell, &Cell::gameEnded);

            m_grid->addWidget(cell, y, x);
            m_cells << cell;
        }
    }

    int numMines = (m_numRows * m_numCols) * 0.20;

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

        Cell* cell = cellAt(QPoint(row, col));

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
    return m_cells[coord.y() * m_numRows + coord.x()];
}

QList<QPoint> MainWindow::neighborsOf(QPoint coord) const
{
    static QList<QPoint> offsets{
        QPoint(-1, -1), QPoint(-1, 0), QPoint(-1, 1),
        QPoint(0, -1), /* QPoint(0, 0), */ QPoint(0, 1),
        QPoint(1, -1), QPoint(1, 0), QPoint(1, 1),
    };

    QList<QPoint> result;

    for (const auto& offset : offsets)
    {
        QPoint neighbor = coord + offset;
        if (neighbor.x() >= 0 && neighbor.x() < m_numCols && neighbor.y() >= 0 && neighbor.y() < m_numRows)
        {
            result << neighbor;
        }
    }

    return result;
}

bool MainWindow::isBoardSolved() const
{
    for (const auto cell : m_cells)
    {
        if (cell->isMine()) continue;
        if (cell->isRevealed()) continue;

        return false;
    }

    return true;
}

void MainWindow::win()
{
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
