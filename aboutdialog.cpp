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

#include "aboutdialog.h"

#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QString>
#include <QVBoxLayout>

namespace {

constexpr const char* kAttributionText =
    "<small>"
    "<a href=\"https://usefulicons.com/glyph-16x16/naval-mine\">Naval Mine</a>"
    " by Arthur Shlain"
    " is licensed under <a href=\"https://creativecommons.org/licenses/by/3.0/\">CC BY 3.0</a>"
    "</small>";

}

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog::QDialog(parent)
{
    setWindowTitle(tr("About Mines"));

    QLabel* logoWidget = new QLabel(this);
    QPixmap logo(":icons/mine.svg");
    logoWidget->setPixmap(logo.scaled(60, 60));

    QLabel* attr = new QLabel(this);
    attr->setText(tr(kAttributionText));
    attr->setTextFormat(Qt::RichText);
    attr->setTextInteractionFlags(Qt::TextBrowserInteraction);
    attr->setOpenExternalLinks(true);

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->setAlignment(Qt::AlignCenter);

    vbox->addWidget(logoWidget, 0, Qt::AlignCenter);
    vbox->addWidget(new QLabel(tr("Mines")), 0, Qt::AlignCenter);
    vbox->addWidget(new QLabel(tr("Copyright 2024 Benjamin Bader")), 0, Qt::AlignCenter);
    vbox->addStretch(1);
    vbox->addWidget(attr, 0, Qt::AlignCenter);

    setLayout(vbox);
}
