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
#include "imageview.h"


ImageView::ImageView(QWidget *parent) 
    : QGLWidget(QGLFormat(QGL::NoDepthBuffer | QGL::DoubleBuffer), parent)
{
    m_zoom = 1.0;
    m_drag = false;
}


ImageView::~ImageView() {
}


void ImageView::setOrigin( const QPoint& origin ) {
    m_origin = origin;
}


void ImageView::setZoom(float zoom) {
    m_zoom = zoom;
}


void ImageView::mousePressEvent( QMouseEvent *e ) {
    if (e->buttons() == Qt::LeftButton) {
        m_drag = true;
        m_dragPos = e->pos();
        m_dragOrigin = m_origin;
    }
}


void ImageView::mouseReleaseEvent( QMouseEvent *e ) {
    if (m_drag && (e->button() == Qt::LeftButton)) {
        m_drag = false;
    }
    updateGL();
}


void ImageView::mouseMoveEvent( QMouseEvent *e ) {
    if (m_drag) {
        m_origin = m_dragOrigin + e->pos() - m_dragPos;
        update();
    }
}


void ImageView::wheelEvent(QWheelEvent *e) {
    QSize sz = size();
    float u = e->delta() / 120.0 / 4.0;
    if (u < -0.5) u = -0.5;
    if (u > 0.5) u = 0.5;
    m_origin *= ( 1 + u);
    setZoom(m_zoom * ( 1 + u));
    updateGL();
}


void ImageView::zoomIn() {
    setZoom(m_zoom * 2.0);
    updateGL();
}


void ImageView::zoomOut() {
    setZoom(m_zoom * 0.5);
    updateGL();
}


void ImageView::reset() {
    setZoom(1.0);
    setOrigin(QPoint(0,0));
    updateGL();
}
