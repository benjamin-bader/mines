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

#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QDataStream>
#include <QSettings>

class GameBoard
{
    int m_rows{};
    int m_cols{};
    int m_mines{};

    friend QDataStream& operator<<(QDataStream&, const GameBoard&);
    friend QDataStream& operator>>(QDataStream&, GameBoard&);

public:
    explicit GameBoard() = default;
    explicit constexpr GameBoard(int rows, int cols, int mines)
        : m_rows(rows)
        , m_cols(cols)
        , m_mines(mines)
    {}

    GameBoard(const GameBoard&) = default;
    GameBoard(GameBoard&&) = default;

    GameBoard& operator=(const GameBoard&) = default;
    GameBoard& operator=(GameBoard&&) = default;

    virtual ~GameBoard() = default;

    auto operator<=>(const GameBoard& that) const = default;

    int rows() const { return m_rows; }
    int cols() const { return m_cols; }
    int mines() const { return m_mines; }

    void save(QSettings& settings);
    void load(QSettings& settings);
};

QDataStream& operator<<(QDataStream&, const GameBoard&);
QDataStream& operator>>(QDataStream&, GameBoard&);

Q_DECLARE_METATYPE(GameBoard)

#endif // GAMEBOARD_H
