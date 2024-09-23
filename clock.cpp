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

#include "clock.h"

#include <chrono>

namespace {

constexpr auto kInterval = std::chrono::seconds{1};

}

Clock::Clock(QObject *parent)
    : QObject{parent}
    , m_timer(new QTimer(this))
    , m_elapsed(0)
    , m_active(false)
{
    m_timer->setInterval(kInterval);
    connect(m_timer, &QTimer::timeout, this, &Clock::onTick);
}

int Clock::getElapsed() const
{
    return m_elapsed;
}

void Clock::resume()
{
    if (!m_active)
    {
        m_active = true;
        m_timer->start();
    }
}

void Clock::pause()
{
    if (m_active)
    {
        m_active = false;
        m_timer->stop();

        emit paused();
    }
}

void Clock::reset()
{
    if (m_active)
    {
        m_active = false;
        m_timer->stop();
    }

    m_elapsed = 0;

    emit didReset();
}

void Clock::onTick()
{
    m_elapsed++;
    emit tick(m_elapsed);
}
