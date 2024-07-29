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

#include "mainwindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName(QObject::tr("Mines"));
    QCoreApplication::setApplicationVersion("0.1.0");
    QCoreApplication::setOrganizationName("com.bendb");
    QCoreApplication::setOrganizationDomain("com.bendb");

    QTranslator translator;
    if (translator.load(QLocale::system(), "Mines", "_", ":i18n/"))
    {
        a.installTranslator(&translator);
    }

    MainWindow w;
    w.show();
    return a.exec();
}
