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

#ifndef MINEFIELD_H
#define MINEFIELD_H

#include "cell.h"
#include "gameboard.h"

#include <QList>
#include <QPoint>
#include <QWidget>

/**
 * @brief The MineField class implements the game's logic and core UI.
 */
class MineField : public QWidget
{
    Q_OBJECT

    GameBoard m_board;
    QList<Cell*> m_cells;
    bool m_started;

public:
    explicit MineField(GameBoard board, QWidget *parent = nullptr);

signals:
    void gameStarted();
    void gameWon();
    void gameLost();

private slots:
    void cellRevealed(const QPoint& coord);

private:
    bool isBoardSolved() const;
    bool isCorner(const QPoint& point) const;

    int rows() const;
    int cols() const;

    Cell* cellAt(const QPoint& coord);

    void win();
    void lose();
};

#endif // MINEFIELD_H
