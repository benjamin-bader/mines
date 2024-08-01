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
