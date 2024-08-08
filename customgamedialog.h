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

#ifndef CUSTOMGAMEDIALOG_H
#define CUSTOMGAMEDIALOG_H

#include "gameboard.h"

#include <QDialog>
#include <QObject>
#include <QSpinBox>

class CustomGameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CustomGameDialog(const GameBoard& currentBoard, QWidget* parent = nullptr);

    GameBoard getBoard() const;

private slots:
    void inputsChanged();

private:
    QSpinBox* m_rows;
    QSpinBox* m_cols;
    QSpinBox* m_mines;
};

#endif // CUSTOMGAMEDIALOG_H
