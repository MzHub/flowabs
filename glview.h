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

#include "imageview.h"
#include "glsl_program.h"
#ifdef HAVE_QUICKTIME
#include "quicktime.h"
#endif

class GLView : public ImageView {
    Q_OBJECT
public:
    GLView(QWidget *parent);
    ~GLView();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    bool open(const QString& fileName);
    bool save(const QString& fileName);

	void process();
    void updateAll();

    #ifdef HAVE_QUICKTIME
    bool record(const QString& fileName);
    bool m_haveQuickTime;
	quicktime_player *m_player;
    #endif
    QImage m_image;
    GLuint  m_fbo;
    texture_2d m_dst;
    texture_2d m_noise;

	QString fileName;
    int	    preview;
    float   sst_sigma;      
    int     bf_ne;          
    int     bf_na;          
    float   bf_sigma_d;     
    float   bf_sigma_r;     
	int     fdog_type;
    int     fdog_n;
    float   fdog_sigma_e;     
    float   fdog_sigma_r;     
    float   fdog_sigma_m;
    float   fdog_tau;         
    float   fdog_phi;         
	float	fdog_color[3];
    int     cq_nbins;       
    float   cq_phi_q;       
    int     cq_filter;
    int     fs_type;
    float   fs_sigma;

signals:
	void playerChanged();
};
