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

#ifndef HEADERBAR_H
#define HEADERBAR_H

#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QWidget>

class HeaderBar : public QWidget
{
    Q_OBJECT

    QLabel* m_clockView;
    QPushButton* m_startButton;

public:
    explicit HeaderBar(QWidget *parent = nullptr);

public slots:
    void updateClock(int elapsed);
    void gameStarted();
    void gameEnded();

signals:
    void startButtonPushed();
};

#endif // HEADERBAR_H
