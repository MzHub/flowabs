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
#pragma once

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

public slots:
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionRecord_triggered();
    void on_actionOpenSettings_triggered();
    void on_actionSaveSettings_triggered();
    void on_actionAbout_triggered();

    void on_fdog_colorButton_clicked();
    void do_update();

    #ifdef HAVE_QUICKTIME
    void on_timeSlider_valueChanged();
    void on_glw_playerChanged();
    #endif

    void openSettings(const QString& fileName);
    void saveSettings(const QString& fileName);

public:
    Ui_MainWindow ui;
    QShortcut m_quit;
    QColor m_fdog_color;
};
