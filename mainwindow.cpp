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
#include "glview.h"


MainWindow::MainWindow() 
    : QMainWindow(),
      m_quit(Qt::Key_Escape, this, SLOT(close()), 0, Qt::ApplicationShortcut)
{
    ui.setupUi(this);

    QList<QWidget*> widgets = this->findChildren<QWidget*>();
    for (int i = 0; i < widgets.size(); ++i) {
        QWidget *w = widgets.at(i);
        if (w->inherits("QAbstractSpinBox")) {
            this->connect(w, SIGNAL(valueChanged(const QString&)), SLOT(do_update()));
        }
        if (w->inherits("QComboBox")) {
            this->connect(w, SIGNAL(activated(int)), SLOT(do_update()));
        }
        if (w->inherits("QCheckBox")) {
            this->connect(w, SIGNAL(stateChanged(int)), SLOT(do_update()));
        }
        if (w->inherits("QGroupBox")) {
            this->connect(w, SIGNAL(toggled(bool)), SLOT(do_update()));
        }

        #ifdef Q_WS_MAC
        if (w->inherits("QLabel")) {
            w->setFont(QApplication::font());
        }
        #endif
    }
}


MainWindow::~MainWindow() {
}


void MainWindow::on_actionOpen_triggered() {
    QFileInfo fi(ui.glw->fileName);
    QString fileName = QFileDialog::getOpenFileName(this, "Open", fi.absolutePath(), 
        "Images and Videos (*.png *.bmp *.tiff *.tif;*.avi;*.mov;*.mp4);;All files (*.*)");
    if (!fileName.isEmpty()) {
        ui.glw->open(fileName);
    }
}


void MainWindow::on_actionSave_triggered() {
    QFileInfo fi(ui.glw->fileName);
    QString fileName = QFileDialog::getSaveFileName(this, "Save", fi.absolutePath() + "/" + fi.baseName() + "-out.png", 
        "Image (*.png);;All files (*.*)");
    if (!fileName.isEmpty()) {
        if (ui.glw->save(fileName)) {
            //QFileInfo fi(fileName);
            //saveSettings(fi.path() + "/" + fi.baseName() + ".ini");
        }
    }
}


void MainWindow::on_actionRecord_triggered() {
    #ifdef HAVE_QUICKTIME    
    QFileInfo fi(ui.glw->fileName);
    QString fileName = QFileDialog::getSaveFileName(this, "Record", fi.absolutePath() + "/" + fi.baseName() + "-out.mov", 
        "QuickTime Movie (*.mov);;All files (*.*)");
    if (!fileName.isEmpty()) {
        if (ui.glw->record(fileName)) {
            //QFileInfo fi(fileName);
            //saveSettings(fi.path() + "/" + fi.baseName() + ".ini");
        }
    }
    #endif
}


void MainWindow::on_actionOpenSettings_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Settings", QString(), 
        "Settings (*.ini);;All files (*.*)");
    if (!fileName.isEmpty()) {
        openSettings(fileName);
    }
}


void MainWindow::on_actionSaveSettings_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Settings", QString(), 
        "Settings (*.ini);;All files (*.*)");
    if (!fileName.isEmpty()) {
        saveSettings(fileName);
    }
}


void MainWindow::on_actionAbout_triggered() {
    QMessageBox msgBox;
    msgBox.setWindowTitle("About");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText("<b>Real-time Flow-based Image Abstraction</b>");
    msgBox.setInformativeText(
        "<html><body>" \
        "Copyright (C) 2008-2011 Computer Graphics Systems Group at " \
        "the Hasso-Plattner-Institut, Potsdam, Germany &lt;<a href='http://www.hpi3d.de'>www.hpi3d.de</a>&gt;" \
        "<br/><br/>" \
        "Author: Jan Eric Kyprianidis &lt;<a href='http://www.kyprianidis.com'>www.kyprianidis.com</a>&gt;<br/>" \
        "Date: " __DATE__ "<br/><br/>"
        "This program is free software: you can redistribute it and/or modify " \
        "it under the terms of the GNU General Public License as published by " \
        "the Free Software Foundation, either version 3 of the License, or " \
        "(at your option) any later version.<br/><br/>" \
        "This program is distributed in the hope that it will be useful, " \
        "but WITHOUT ANY WARRANTY; without even the implied warranty of " \
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the " \
        "GNU General Public License for more details."
        "<br/><br/>" \
        "Related Publications:" \
        "<ul><li>" \
        "Kyprianidis, J. E., &amp; D&ouml;llner, J. (2009). " \
        "Real-Time Image Abstraction by Directed Filtering. "\
        "In W. Engel (Ed.), <em>ShaderX7 - Advanced Rendering Techniques</em>. " \
        "Charles River Media." \
        "</li><li>" \
        "Kyprianidis, J. E., &amp; D&ouml;llner, J. (2008). " \
        "Image Abstraction by Structure Adaptive Filtering. " \
        "In <em>Proc. EG UK Theory and Practice of Computer Graphics</em>, pp. 51-58." \
        "</li></ul>" \
        "Test image courtesy of Anthony Santella." \
        "</body></html>"
        );
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}


void MainWindow::on_fdog_colorButton_clicked() {
    m_fdog_color = QColorDialog::getColor(m_fdog_color, this);
    QPalette palette = ui.fdog_color->palette();
    palette.setColor(ui.fdog_color->backgroundRole(), m_fdog_color);
    palette.setColor(ui.fdog_color->foregroundRole(), m_fdog_color);
    ui.fdog_color->setPalette(palette);
    do_update();
}


void MainWindow::openSettings(const QString& fileName) {
    QSettings settings(fileName, QSettings::IniFormat);
    QList<QWidget*> widgets = this->findChildren<QWidget*>();
    for (int i = 0; i < widgets.size(); ++i) {
        QWidget *w = widgets.at(i);

        QSpinBox *sb = qobject_cast<QSpinBox*>(w);
        if (sb) {
            sb->setValue( settings.value(sb->objectName()).toInt() );
        }

        QDoubleSpinBox *dsb = qobject_cast<QDoubleSpinBox*>(w);
        if (dsb) {
            dsb->setValue( settings.value(dsb->objectName()).toDouble() );
        }
        
        QComboBox *cb = qobject_cast<QComboBox*>(w);
        if (cb) {
            cb->setCurrentIndex( settings.value(cb->objectName()).toInt() );
        }
    }
    {
        m_fdog_color = QColor(settings.value("fdog_color").toString());
        QPalette palette = ui.fdog_color->palette();
        palette.setColor(ui.fdog_color->backgroundRole(), m_fdog_color);
        palette.setColor(ui.fdog_color->foregroundRole(), m_fdog_color);
        ui.fdog_color->setPalette(palette);
    }
    do_update();
}


void MainWindow::saveSettings(const QString& fileName) {
    QSettings settings(fileName, QSettings::IniFormat);
    QList<QWidget*> widgets = this->findChildren<QWidget*>();
    for (int i = 0; i < widgets.size(); ++i) {
        QWidget *w = widgets.at(i);

        QSpinBox *sb = qobject_cast<QSpinBox*>(w);
        if (sb) {
            settings.setValue(sb->objectName(), sb->value());
        }

        QDoubleSpinBox *dsb = qobject_cast<QDoubleSpinBox*>(w);
        if (dsb) {
            settings.setValue(dsb->objectName(), dsb->value());
        }
        
        QComboBox *cb = qobject_cast<QComboBox*>(w);
        if (cb) {
            settings.setValue(cb->objectName(), cb->currentIndex());
        }
    }
    settings.setValue("fdog_color", m_fdog_color.name());
}


void MainWindow::do_update() {
    ui.glw->preview = ui.preview->currentIndex();
    ui.glw->sst_sigma = ui.sst_sigma->value();
    ui.glw->bf_ne = ui.bf_ne->value();
    ui.glw->bf_na = ui.bf_na->value();
    ui.glw->bf_sigma_d = ui.bf_sigma_d->value();
    ui.glw->bf_sigma_r = ui.bf_sigma_r->value() / 100.0;
    ui.glw->fdog_type = ui.fdog_type->currentIndex(); 
    ui.glw->fdog_n = ui.fdog_n->value();
    ui.glw->fdog_sigma_e = ui.fdog_sigma_e->value();
    ui.glw->fdog_sigma_r = ui.fdog_sigma_r->value();
    ui.glw->fdog_sigma_m = ui.fdog_sigma_m->value();
    ui.glw->fdog_tau = ui.fdog_tau->value();
    ui.glw->fdog_phi = ui.fdog_phi->value();
    ui.glw->fdog_color[0] = m_fdog_color.redF();
    ui.glw->fdog_color[1] = m_fdog_color.greenF();
    ui.glw->fdog_color[2] = m_fdog_color.blueF();
    ui.glw->cq_nbins = ui.cq_nbins->value(); 
    ui.glw->cq_phi_q = ui.cq_phi_q->value(); 
    ui.glw->cq_filter = ui.cq_filter->currentIndex();
    ui.glw->fs_type = ui.fs_type->currentIndex(); 
    ui.glw->fs_sigma = ui.fs_sigma->value(); 
    ui.fs_sigma->setEnabled(ui.glw->fs_type == 3);
    ui.fdog_sigma_m->setEnabled(ui.glw->fdog_type == 0);
    ui.glw->updateAll();
}


#ifdef HAVE_QUICKTIME
void MainWindow::on_timeSlider_valueChanged() {
    quicktime_player *player = ui.glw->m_player;
    if (player) {
        int time = ui.timeSlider->value();
        player->set_time(time);
        player->update();
        ui.glw->updateAll();
    }
}


void MainWindow::on_glw_playerChanged() {
    quicktime_player *player = ui.glw->m_player;
    if (player) {
        ui.actionRecord->setEnabled(true);
        ui.timeSlider->setEnabled(player->get_frame_count() > 1);
        ui.timeSlider->setRange(0, player->get_duration());
        ui.timeSlider->setValue(player->get_time());
    } else {
        ui.actionRecord->setEnabled(false);
        ui.timeSlider->setEnabled(false);
    }
}
#endif
