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

#include "cell.h"

#include <QBrush>
#include <QFontMetrics>
#include <QColor>
#include <QPainter>
#include <QPainterPath>

#include <array>

namespace Brushes
{

QBrush DarkerBackground(Qt::darkGray);
QBrush DarkBackground(Qt::gray);
QBrush Background(Qt::lightGray);
QBrush Highlight(Qt::white);
QBrush Test(Qt::red);

const auto labelColors = std::array {
    Qt::red, // zero value should never be used
    Qt::blue,
    Qt::green,
    Qt::red,
    Qt::darkBlue,
    Qt::darkRed,
    Qt::darkCyan,
    Qt::black,
    Qt::magenta,
};

} // namespace Brushes

Cell::Cell(int x, int y, QWidget *parent)
    : QPushButton{parent}
    , m_revealed(false)
    , m_flagged(false)
    , m_leftMouseDown(false)
    , m_rightMouseDown(false)
    , m_x(x)
    , m_y(y)
    , m_numNeighboringMines(0)
{
    setAttribute(Qt::WA_LayoutUsesWidgetRect);
}

void Cell::setX(int x)
{
    m_x = x;
}

void Cell::setY(int y)
{
    m_y = y;
}

void Cell::setNumNeighboringMines(int numNeighboringMines)
{
    Q_ASSERT(numNeighboringMines >= -1);
    Q_ASSERT(numNeighboringMines <= 9);
    m_numNeighboringMines = numNeighboringMines;
}

void Cell::toggleFlag()
{
    if (m_revealed)
    {
        return;
    }

    m_flagged = !m_flagged;
    repaint();
}

void Cell::reveal()
{
    if (m_revealed)
    {
        return;
    }
    m_revealed = true;

    auto revelation = std::make_shared<Revelation>();
    revelation->safe = m_numNeighboringMines != -1;
    revelation->x = m_x;
    revelation->y = m_y;
    revelation->numNeighboringMines = m_numNeighboringMines;

    emit revealed(revelation);

    update();
}

void Cell::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        m_rightMouseDown = true;
    }
    else if (event->button() == Qt::LeftButton)
    {
        m_leftMouseDown = true;
    }

    QPushButton::mousePressEvent(event);
}

void Cell::mouseReleaseEvent(QMouseEvent* event)
{
    auto pos = event->pos();
    auto rect = this->rect();
    bool releasedInThisCell = rect.contains(pos, true);

    if (event->button() == Qt::RightButton)
    {
        m_rightMouseDown = false;

        if (releasedInThisCell)
        {
            toggleFlag();
            event->accept();
        }
    }
    else if (event->button() == Qt::LeftButton)
    {
        m_leftMouseDown = false;

        if (releasedInThisCell)
        {
            reveal();
            event->accept();
        }
    }

    QPushButton::mouseReleaseEvent(event);
}

void Cell::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    const QRectF rect = event->rect().toRectF();

    double dy = rect.height() * 0.1;
    double dx = rect.width() * 0.1;

    QPointF topLeftOut(rect.topLeft());
    QPointF topRightOut(rect.topRight());
    QPointF bottomLeftOut(rect.bottomLeft());
    QPointF bottomRightOut(rect.bottomRight());

    QPointF topLeftIn(topLeftOut.x() + dx, topLeftOut.y() + dy);
    QPointF topRightIn(topRightOut.x() - dx, topRightOut.y() + dy);
    QPointF bottomLeftIn(bottomLeftOut.x() + dx, bottomLeftOut.y() - dy);
    QPointF bottomRightIn(bottomRightOut.x() - dx, bottomRightOut.y() - dy);

    if (m_revealed)
    {
        painter.fillRect(rect, Brushes::Background);

        painter.setPen(Qt::gray);
        painter.drawLine(rect.topLeft(), rect.topRight());
        painter.drawLine(rect.topRight(), rect.bottomRight());
        painter.drawLine(rect.bottomRight(), rect.bottomLeft());
        painter.drawLine(rect.bottomLeft(), rect.topLeft());

        if (m_numNeighboringMines == 0)
        {
            return;
        }

        QString label;
        if (m_numNeighboringMines == -1)
        {
            label = "B";
            painter.fillRect(rect, QBrush(Qt::red));
            painter.setPen(Qt::black);
        }
        else
        {
            label = QString::number(m_numNeighboringMines);
            painter.setPen(Brushes::labelColors[m_numNeighboringMines]);
        }

        QFontMetrics fm = painter.fontMetrics();
        QRect br = fm.tightBoundingRect(label);
        QPoint labelStart(
            rect.center().x() - round(br.width() / 2.0),
            rect.center().y() + round(br.height() / 2.0));

        painter.drawText(labelStart, label);
    }
    else
    {
        QPainterPath highlight;
        highlight.moveTo(bottomLeftOut);
        highlight.lineTo(bottomLeftIn);
        highlight.lineTo(topLeftIn);
        highlight.lineTo(topRightIn);
        highlight.lineTo(topRightOut);
        highlight.lineTo(topLeftOut);
        highlight.lineTo(bottomLeftOut);

        QPainterPath lowlight;
        lowlight.moveTo(bottomLeftOut);
        lowlight.lineTo(bottomRightOut);
        lowlight.lineTo(topRightOut);
        lowlight.lineTo(topRightIn);
        lowlight.lineTo(bottomRightIn);
        lowlight.lineTo(bottomLeftIn);
        lowlight.lineTo(bottomLeftOut);

        if (m_leftMouseDown)
        {
            painter.fillRect(rect, Brushes::DarkerBackground);
            painter.fillPath(highlight, Brushes::DarkBackground);
            painter.fillPath(lowlight, Brushes::Background);
        }
        else
        {
            painter.fillRect(rect, Brushes::Background);
            painter.fillPath(highlight, Brushes::Highlight);
            painter.fillPath(lowlight, Brushes::DarkBackground);
        }

        if (m_flagged)
        {
            QString label = QStringLiteral("F");
            QFontMetrics fm = painter.fontMetrics();
            QRect br = fm.tightBoundingRect(label);
            QPoint labelStart(
                rect.center().x() - round(br.width() / 2.0),
                rect.center().y() + round(br.height() / 2.0));

            painter.drawText(labelStart, label);
        }
    }

//    painter.save();
    painter.setPen(Qt::gray);
    painter.drawLine(rect.topLeft(), rect.topRight());
    painter.drawLine(rect.topRight(), rect.bottomRight());
    painter.drawLine(rect.bottomRight(), rect.bottomLeft());
    painter.drawLine(rect.bottomLeft(), rect.topLeft());
//    painter.restore();
}