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

#include "gameboard.h"

void GameBoard::save(QSettings& settings)
{
    settings.beginGroup("board");
    settings.setValue("rows", m_rows);
    settings.setValue("cols", m_cols);
    settings.setValue("mines", m_mines);
    settings.endGroup();
}

void GameBoard::load(QSettings& settings)
{
    settings.beginGroup("board");
    m_rows = settings.value("rows", 15).toInt();
    m_cols = settings.value("cols", 15).toInt();
    m_mines = settings.value("mines", 45).toInt();
    settings.endGroup();
}

bool operator==(const GameBoard& lhs, const GameBoard& rhs)
{
    return lhs.rows() == rhs.rows()
    && lhs.cols() == rhs.cols()
        && lhs.mines() == rhs.mines();
}

QDataStream& operator<<(QDataStream& stream, const GameBoard& board)
{
    return stream << board.m_rows << board.m_cols << board.m_mines;
}

QDataStream& operator>>(QDataStream& stream, GameBoard& board)
{
    return stream >> board.m_rows >> board.m_cols >> board.m_mines;
}
