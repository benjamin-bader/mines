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

#include "aboutdialog.h"
#include "cell.h"
#include "clock.h"
#include "gameboard.h"
#include "headerbar.h"

#include <QAction>
#include <QActionGroup>
#include <QGridLayout>
#include <QList>
#include <QMainWindow>
#include <QPoint>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void beginCustomGame(bool checked);
    void showAboutDialog();
    void clockTicked(int elapsed);

private:
    void initializeActions();
    void initializeMenu();
    void initializeGame(GameBoard board);
    void initializeGrid();

    void updateMenuCheckboxes();

    int rows() const;
    int cols() const;
    int mines() const;

    void win();
    void lose();

    GameBoard m_board;
    QList<Cell*> m_cells;

    QActionGroup* m_gameSizeGroup;
    QAction* m_smallGame;
    QAction* m_mediumGame;
    QAction* m_largeGame;
    QAction* m_customGame;

    AboutDialog* m_about;
    Clock* m_clock;
    HeaderBar* m_headerBar;
};

#endif // MAINWINDOW_H
