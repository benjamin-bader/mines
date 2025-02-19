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

#include "headerbar.h"

#include <QHBoxLayout>

HeaderBar::HeaderBar(QWidget *parent)
    : QWidget{parent}
    , m_clockView{new QLabel{this}}
    , m_startButton{new QPushButton{this}}
{
    m_clockView->setText("000");

    m_startButton->setEnabled(false);
    connect(m_startButton, &QPushButton::clicked, this, &HeaderBar::startButtonPushed);

    QHBoxLayout* hbox = new QHBoxLayout{this};

    hbox->addWidget(m_clockView);
    hbox->addStretch();
    hbox->addWidget(m_startButton);

    setLayout(hbox);
}

void HeaderBar::updateClock(int elapsed)
{
    QString text = QString::number(elapsed);
    m_clockView->setText(text);
}

void HeaderBar::gameStarted()
{
    m_clockView->setText("000");
}

void HeaderBar::gameEnded()
{
    m_startButton->setEnabled(true);
}

