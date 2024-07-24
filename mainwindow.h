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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cell.h"

#include <QGridLayout>
#include <QList>
#include <QMainWindow>
#include <QPoint>

#include <memory>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

private slots:
    void cellRevealed(const std::shared_ptr<Revelation>& revelation);

private:
    void initializeMenu();
    void initializeGrid();
    Cell* cellAt(int x, int y);
    QList<QPoint> neighborsOf(int x, int y) const;
    bool isBoardSolved() const;

    void win();
    void lose();

    int m_numRows;
    int m_numCols;
    QGridLayout* m_grid;
    QList<Cell*> m_cells;
};
#endif // MAINWINDOW_H
