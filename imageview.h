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

class ImageView : public QGLWidget {
    Q_OBJECT
public:
    ImageView(QWidget *parent);
    ~ImageView();

    void mousePressEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void wheelEvent(QWheelEvent *e);

    void setOrigin(const QPoint& origin);
    void setZoom(float zoom);

    float zoom() const { return m_zoom; }
    QPoint origin() const { return m_origin; }

public slots:
    void zoomIn();
    void zoomOut();
    void reset();

private:
    QPoint m_origin;
    float m_zoom;
    bool m_drag;
    QPoint m_dragPos;
    QPoint m_dragOrigin;
};
