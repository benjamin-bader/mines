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

#ifndef CELL_H
#define CELL_H

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPushButton>

#include <memory>

struct Revelation
{
    bool safe;
    int x;
    int y;
    int numNeighboringMines;
};

class Cell : public QPushButton
{
    Q_OBJECT

public:
    explicit Cell(int x, int y, QWidget *parent = nullptr);

    void setX(int x);
    void setY(int y);
    void setNumNeighboringMines(int numNeighboringMines); // -1 means that this cell is a mine
    void toggleFlag();

    bool isMine() const
    {
        return m_numNeighboringMines == -1;
    }

    bool isRevealed() const
    {
        return m_revealed;
    }

public slots:
    void reveal();
    void gameEnded();

signals:
    void revealed(const std::shared_ptr<Revelation>& revelation);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    bool m_revealed;
    bool m_flagged;
    bool m_gameOver;
    bool m_leftMouseDown;
    bool m_rightMouseDown;
    int m_x;
    int m_y;
    int m_numNeighboringMines; // -1 means "mine"
};

#endif // CELL_H
