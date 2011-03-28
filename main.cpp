//
// by Jan Eric Kyprianidis <www.kyprianidis.com>
// Copyright (C) 2008-2011 Computer Graphics Systems Group at the
// Hasso-Plattner-Institut, Potsdam, Germany <www.hpi3d.de>
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
#include "mainwindow.h"
#ifdef HAVE_QUICKTIME    
#include "quicktime.h"
#endif

static const char* DEFAULT_FILE = ":/test_512x512.png";


int main(int argc, char **argv) {
    QApplication::setOrganizationDomain("jkyprian.hpi3d.de");
    QApplication::setOrganizationName("jkyprian");
    QApplication::setApplicationName("flowabs");
    QApplication app(argc, argv);
    QApplication::setStyle(new QPlastiqueStyle);
    QApplication::setPalette(QApplication::style()->standardPalette());

    #ifndef Q_WS_WIN
    QFont font = QApplication::font();
    font.setPixelSize(11);
    QApplication::setFont(font);
    #endif

    MainWindow mw;
    QSettings settings;
    mw.restoreGeometry(settings.value("mw").toByteArray());
    mw.openSettings(":/default.ini");
    mw.show();
    mw.ui.glw->open(DEFAULT_FILE);

    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    int result = app.exec();

    settings.setValue("mw", mw.saveGeometry());
    return result;
}
