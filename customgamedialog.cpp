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

#include "customgamedialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>

CustomGameDialog::CustomGameDialog(const GameBoard& currentBoard, QWidget* parent)
    : QDialog::QDialog(parent)
{
    setWindowTitle(tr("Custom Game"));

    QSizePolicy policy;
    policy.setHorizontalPolicy(QSizePolicy::Fixed);

    m_rows = new QSpinBox(this);
    m_rows->setMinimumWidth(40);
    m_rows->setRange(3, 75);
    m_rows->setValue(currentBoard.rows());
    connect(m_rows, &QSpinBox::valueChanged, this, &CustomGameDialog::inputsChanged);

    m_cols = new QSpinBox(this);
    m_cols->setMinimumWidth(40);
    m_cols->setRange(3, 75);
    m_cols->setValue(currentBoard.cols());
    connect(m_cols, &QSpinBox::valueChanged, this, &CustomGameDialog::inputsChanged);

    m_mines = new QSpinBox(this);
    m_mines->setMinimumWidth(40);
    m_mines->setRange(1, 5000); // temporary four-digit value for determing field size; see hack below
    m_mines->setValue(currentBoard.mines());

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QFormLayout* layout = new QFormLayout(this);
    layout->addRow(new QLabel(tr("Rows: ")), m_rows);
    layout->addRow(new QLabel(tr("Columns: ")), m_cols);
    layout->addRow(new QLabel(tr("Mines: ")), m_mines);
    layout->addRow(buttons);

    // hack to prevent the mines spinbox from resizing when width or height change
    layout->update();
    m_mines->setMinimumWidth(m_mines->minimumSizeHint().width());
    m_mines->setMaximumWidth(m_mines->minimumSizeHint().width());
    m_mines->setMaximum((m_rows->value() * m_cols->value() - 4));

    setLayout(layout);
}

GameBoard CustomGameDialog::getBoard() const
{
    return GameBoard{m_rows->value(), m_cols->value(), m_mines->value()};
}

void CustomGameDialog::inputsChanged()
{
    int maxMines = m_rows->value() * m_cols->value() - 4;
    m_mines->setRange(1, maxMines);
}
