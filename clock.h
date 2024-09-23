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

#ifndef CLOCK_H
#define CLOCK_H

#include <QObject>
#include <QTimer>

/**
 * @brief A QTimer with a counter of elapsed seconds.
 */
class Clock : public QObject
{
    Q_OBJECT

    QTimer* m_timer;
    int m_elapsed;
    bool m_active;

public:
    explicit Clock(QObject *parent = nullptr);

    int getElapsed() const;

signals:
    void tick(int elapsed);
    void paused();
    void didReset();

public slots:
    void resume();
    void pause();
    void reset();

private slots:
    void onTick();
};

#endif // CLOCK_H
