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
#include "glview.h"
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif


GLView::GLView(QWidget *parent) : ImageView(parent) {
    preview = 0;
    #ifdef HAVE_QUICKTIME
    m_player = 0;
    #endif
}


GLView::~GLView() {
}


void GLView::initializeGL() {
    #ifdef HAVE_QUICKTIME
    m_haveQuickTime = quicktime_init();
    if (!m_haveQuickTime) {
        QMessageBox::warning(NULL, "Warning", "<a href='http://www.apple.com/quicktime/download'>QuickTime 7 Pro</a> not found!<br>");
    }
    #endif

    if (!GLEE_VERSION_2_0 ||
		!GLEE_EXT_framebuffer_object ||
		!GLEE_ARB_texture_float ||
		!GLEE_ARB_texture_rectangle ||
		!GLEE_EXT_bgra) {
		QMessageBox::critical(this, "Error", "OpenGL 2.0 Graphics Card with EXT_framebuffer_object, ARB_texture_rectangle, ARB_texture_float and EXT_bgra required");
	}
	
	bool status = true;
    QDir glsl_dir(":/glsl");
    QFileInfoList glsl_list = glsl_dir.entryInfoList();
    for (int i = 0; i < glsl_list.size(); ++i) {
        QFileInfo fi = glsl_list[i];
        std::string fn = fi.fileName().toStdString();
        QFile f(fi.filePath());
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::critical(NULL, "Error", QString("Can't open %1").arg(fi.filePath()));
			exit(1);
        }

        #ifdef WIN32
            OutputDebugStringA(fn.c_str());
            OutputDebugStringA("\n");
        #else
            fprintf(stderr, "%s\n", fn.c_str());
        #endif

        QByteArray src = f.readAll();
        glsl_program glsl(fn.c_str(), true);
        glsl.attach_shader(GL_FRAGMENT_SHADER, src.constData());
        status &= glsl.link();
    }
    if (!status) {
        QMessageBox::critical(this, "Error", "Compiling/Linking shader failed!");
        qApp->quit();
    }

    glGenFramebuffersEXT(1, &m_fbo);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}


void GLView::resizeGL(int, int) {
    updateGL();
}


void GLView::paintGL() {
    QSize sz = size();
    QPoint o = origin();
    float z = zoom();

    glViewport(0, 0, sz.width(), sz.height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-sz.width()/2, sz.width()/2, -sz.height()/2, sz.height()/2);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glScalef(1.0, -1.0, 1.0);
    glTranslatef(o.x(), o.y(), 0.0);
    glScalef(z, z, 1.0);
    
    glClearColor(.8f, .8f, .8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    

    if (m_dst.is_valid()) {
        glBindTexture(GL_TEXTURE_2D, m_dst.get_id());
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(-m_dst.get_width()/2, -m_dst.get_height()/2);
        glTexCoord2f(0, 1); glVertex2f(-m_dst.get_width()/2,  m_dst.get_height()/2);
        glTexCoord2f(1, 1); glVertex2f( m_dst.get_width()/2,  m_dst.get_height()/2);
        glTexCoord2f(1, 0); glVertex2f( m_dst.get_width()/2, -m_dst.get_height()/2);               
        glEnd();
        assert(glGetError() == GL_NO_ERROR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}


bool GLView::open(const QString& fileName) {
    #ifdef HAVE_QUICKTIME
    if (m_player) {
        m_image = QImage();
        delete m_player;
        m_player = 0;
    }
    #endif
    
    m_image = QImage(fileName);
    #ifdef HAVE_QUICKTIME
    if (m_image.isNull() && m_haveQuickTime) {
	    std::string pathStd = fileName.toStdString();
	    #ifdef Q_OS_WIN
	    for (std::string::iterator p = pathStd.begin(); p != pathStd.end(); ++p) if (*p == '/') *p = '\\';
	    #endif
        m_player = quicktime_player::open(pathStd.c_str());
        if (m_player) {
            m_image = QImage((uchar*)m_player->get_buffer(), m_player->get_width(), m_player->get_height(), QImage::Format_RGB32);
            m_player->set_time(0);
            m_player->update();
        }
    } else
    #endif
    {
    	m_image = m_image.convertToFormat(QImage::Format_RGB32);
    }

    if (!m_image.isNull()) {

        int w = m_image.width();
        int h = m_image.height();
        float *noise = new float[w * h];
        {   
            srand(1);
            float *p = noise;
            for (int j = 0; j < h; ++j) {
                for (int i = 0; i < w; ++i) {
                    *p++ = 0.5f + 2.0f * ((float)rand() / RAND_MAX - 0.5); 
                }
            }
            p = noise;
            for (int j = 0; j < h; ++j) {
                *p = (3*p[0] + p[1]) / 4;
                ++p;
                for (int i = 1; i < w-1; ++i) {
                    *p = (p[-1] + 2*p[0] + p[1]) / 4;
                    ++p;
                }
                *p = (p[-1] + 3*p[0]) / 4;
                ++p;
            }
            p = noise;
            for (int i = 0; i < w; ++i) {
                *p = (3*p[0] + p[w]) / 4;
                ++p;
            }
            for (int j = 1; j < h-1; ++j) {
                for (int i = 0; i < w; ++i) {
                    *p = (p[-w] + 2*p[0] + p[w]) / 4;
                    ++p;
                }
            }
            for (int i = 0; i < w; ++i) {
                *p = (p[-w] + 3*p[0]) / 4;
                ++p;
            }
        }
        makeCurrent();
        m_noise = texture_2d(GL_LUMINANCE16F_ARB, w, h, GL_LUMINANCE, GL_FLOAT, noise);
		assert(glGetError() == GL_NO_ERROR);
		delete[] noise;
    } else {
        QMessageBox::critical(this, "Error", QString(
			"QuickTime can't load '%1'. To get further information "\
			"please try load the file with the QuickTime player.").arg(fileName));
    }

	playerChanged();
	setOrigin(QPoint(0,0));
	setZoom(1.0);
	updateAll();
	this->fileName = fileName;

	return !m_image.isNull();
}


bool GLView::save(const QString& fileName) {
	if (!m_dst.is_valid()) return false;
	makeCurrent();

    GLint oldReadBuffer;    
    glGetIntegerv(GL_READ_BUFFER, &oldReadBuffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_dst.get_id(), 0);
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);

    QImage img(m_dst.get_width(), m_dst.get_height(), QImage::Format_RGB32);
    glReadPixels(0,0, m_dst.get_width(), m_dst.get_height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.bits());
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glReadBuffer(oldReadBuffer);

	if (!img.save(fileName)) {
		QMessageBox::critical(this, "Error", "Saving image failed!");
		return false;
	}
	return true;
}


#ifdef HAVE_QUICKTIME
bool GLView::record( const QString& fileName ) {
	if (!m_player) return false;
	
	QString fn(fileName);
	fn.replace('/','\\');

    quicktime_recorder *recorder = quicktime_recorder::create(fn.toLatin1(), 
        m_player->get_width(), m_player->get_height(), m_player->get_fps());
    if (!recorder) {
		QMessageBox::critical(this, "Error", "Creation of QuickTime recorder failed!");
		return false;
	}

	parentWidget()->setEnabled(false);

	QProgressDialog progress("Recording...", "Abort", 0, m_player->get_duration(), this);
	progress.setWindowModality(Qt::WindowModal);

	makeCurrent();

	int nextTime = 0;
	while (nextTime >= 0) {
		progress.setValue(nextTime);
		m_player->set_time(nextTime);
		m_player->update();

		updateAll();
		glFinish();

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_dst.get_id(), 0);
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glReadPixels(0,0, m_dst.get_width(), m_dst.get_height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, recorder->get_buffer());
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		recorder->append_frame();

		nextTime = m_player->get_next_time(nextTime);

		if (progress.wasCanceled())
			break;
	}

	recorder->finish();
	delete recorder;

	parentWidget()->setEnabled(true);
	playerChanged();
	return true;
}
#endif


texture_2d rgb2lab(const texture_2d& src) {
    glsl_program glsl("rgb2lab_fs.glsl");
    texture_2d dst(src.clone_format());
    
    glsl.use();
    glsl.bind_sampler("img", src);
    glsl.set_uniform_2f("img_size", (float)src.get_width(), (float)src.get_height());
    glsl.draw(&dst);
    return dst;
}


texture_2d lab2rgb(const texture_2d& src) {
    glsl_program glsl("lab2rgb_fs.glsl");
    texture_2d dst(src.clone_format());
    
    glsl.use();
    glsl.bind_sampler("img", src);
    glsl.set_uniform_2f("img_size", (float)src.get_width(), (float)src.get_height());
    glsl.draw(&dst);
    return dst;
}


texture_2d gauss_filter(const texture_2d& src, 
                        float sigma) 
{
    glsl_program glsl("gauss_fs.glsl");
    texture_2d dst(src.clone_format());
    
    glsl.use();
    glsl.bind_sampler("img", src);
    glsl.set_uniform_1f("sigma", sigma);
    glsl.set_uniform_2f("img_size", (float)src.get_width(), (float)src.get_height());
    glsl.draw(&dst);
    return dst;
}


texture_2d tangent_flow_map(const texture_2d& src, float sigma) {
    texture_2d tmp0(src.clone_format());
    texture_2d tmp1(src.clone_format());
    texture_2d dst(src.clone_format());
    
    glsl_program sst("sst_fs.glsl");
    sst.use();
    sst.bind_sampler("img", src);
    sst.set_uniform_2f("img_size", (float)src.get_width(), (float)src.get_height());
    sst.draw(&tmp0);

    glsl_program gauss("gauss_fs.glsl");
    gauss.use();
    gauss.bind_sampler("img", tmp0);
    gauss.set_uniform_2f("img_size", (float)src.get_width(), (float)src.get_height());
    gauss.set_uniform_1f("sigma", sigma);
    gauss.draw(&tmp1);                  

    glsl_program tfm("tfm_fs.glsl");
    tfm.use();
    tfm.bind_sampler("img", tmp1);
    tfm.set_uniform_2f("img_size", (float)src.get_width(), (float)src.get_height());
    tfm.draw(&dst);

    return dst;
}


texture_2d orientation_aligned_bilateral_filter(const texture_2d& src,
                                                const texture_2d& tfm, 
                                                int n, 
                                                float sigma_d,     
                                                float sigma_r) 
{
    texture_2d tmp(src.clone_format());
    texture_2d dst(src.clone_format());

    glsl_program glsl("bf_fs.glsl");
    glsl.use();
    glsl.set_uniform_2f("img_size", (float)src.get_width(), (float)src.get_height());
    glsl.set_uniform_1f("sigma_d", sigma_d);
    glsl.set_uniform_1f("sigma_r", sigma_r);
    glsl.bind_sampler("tfm", tfm);

    for (int i = 0; i < n; ++i) {
        glsl.bind_sampler("img", (i == 0)? src : dst, GL_LINEAR);
        glsl.set_uniform_1i("pass", 0);
        glsl.draw(&tmp);

        glsl.bind_sampler("img", tmp, GL_LINEAR);
        glsl.set_uniform_1i("pass", 1);
        glsl.draw(&dst);
    }

    return dst;
}


texture_2d overlay(const texture_2d& edges, 
                   const texture_2d& img)
{
    texture_2d dst(edges.clone_format());

    glsl_program glsl("overlay_fs.glsl");
    glsl.use();
    glsl.bind_sampler("edges", edges);
    glsl.bind_sampler("img", img);
    glsl.set_uniform_2f("img_size", (float)dst.get_width(), (float)dst.get_height());
    glsl.draw(&dst);
 
    return dst;
}


texture_2d fdog_filter(const texture_2d& src, 
                       const texture_2d& tfm,
                       int n,
                       float sigma_e,
                       float sigma_r,
                       float tau,
                       float sigma_m,
                       float phi) 
{
    texture_2d tmp(src.clone_format());
    texture_2d dst(src.clone_format());

    for (int i = 0; i < n; ++i) {
        texture_2d img = (i == 0)? src : overlay(dst, src);
        glsl_program fdog0("fdog0_fs.glsl");
        fdog0.use();
        fdog0.bind_sampler("img", img, GL_LINEAR);
        fdog0.bind_sampler("tfm", tfm, GL_NEAREST);
        fdog0.set_uniform_1f("sigma_e", sigma_e);
        fdog0.set_uniform_1f("sigma_r", sigma_r);
        fdog0.set_uniform_1f("tau", tau);
        fdog0.set_uniform_2f("img_size", (float)src.get_width(), (float)src.get_height());
        fdog0.draw(&tmp);

        glsl_program fdog1("fdog1_fs.glsl");
        fdog1.use();
        fdog1.bind_sampler("img", tmp, GL_LINEAR);
        fdog1.bind_sampler("tfm", tfm, GL_NEAREST);
        fdog1.set_uniform_1f("sigma_m", sigma_m);
        fdog1.set_uniform_1f("phi", phi);
        fdog1.set_uniform_2f("img_size", (float)src.get_width(), (float)src.get_height());
        fdog0.draw(&dst);
    }
    return dst;
}


texture_2d dog_filter(const texture_2d& src, 
					   int n,
					   float sigma_e,
					   float sigma_r,
					   float tau,
					   float phi) 
{
	texture_2d tmp(src.clone_format());
	//texture_2d dst(src.clone_format());

	for (int i = 0; i < n; ++i) {
		texture_2d img = (i == 0)? src : overlay(tmp, src);
		glsl_program dog("dog_fs.glsl");
		dog.use();
		dog.bind_sampler("img", img, GL_NEAREST);
		dog.set_uniform_1f("sigma_e", sigma_e);
		dog.set_uniform_1f("sigma_r", sigma_r);
		dog.set_uniform_1f("tau", tau);
		dog.set_uniform_1f("phi", phi);
		dog.set_uniform_2f("img_size", (float)src.get_width(), (float)src.get_height());
		dog.draw(&tmp);
	}
	return tmp;
}


texture_2d color_quantization(const texture_2d& img,
                              int nbins,
                              float phi_q,
                              int filter)
{
    texture_2d dst(img.clone_format());
    texture_2d tmp(img.clone_format());

    glsl_program cq("color_quantization_fs.glsl");
    cq.use();
    cq.bind_sampler("img", img);
    cq.set_uniform_1i("nbins", nbins);
    cq.set_uniform_1f("phi_q", phi_q);
    cq.set_uniform_2f("img_size", (float)dst.get_width(), (float)dst.get_height());
    cq.draw(&tmp);

    if (filter) {
        glsl_program gauss((filter == 1)? "gauss3x3_fs.glsl" : "gauss5x5_fs.glsl");
        gauss.use();
        gauss.bind_sampler("img", tmp);
        gauss.set_uniform_2f("img_size", (float)dst.get_width(), (float)dst.get_height());
        gauss.draw(&dst);                  
    } else {
        dst = tmp;
    }

    return dst;
}


texture_2d lic_filter(const texture_2d& tfm, 
                      const texture_2d& img,
                      float sigma)
{
    texture_2d dst(tfm.clone_format());

    glsl_program glsl("lic_fs.glsl");
    glsl.use();
    glsl.bind_sampler("tfm", tfm);
    glsl.bind_sampler("img", img);
    glsl.set_uniform_1f("sigma", sigma);
    glsl.set_uniform_2f("img_size", (float)dst.get_width(), (float)dst.get_height());
    glsl.draw(&dst);
 
    return dst;
}


texture_2d mix_filter(const texture_2d& edges, 
                      const texture_2d& img, float edge_color[3])
{
    texture_2d dst(edges.clone_format());

    glsl_program glsl("mix_fs.glsl");
    glsl.use();
    glsl.bind_sampler("edges", edges);
    glsl.bind_sampler("img", img);
    glsl.set_uniform_2f("img_size", (float)dst.get_width(), (float)dst.get_height());
    glsl.set_uniform_3f("edge_color", edge_color[0], edge_color[1], edge_color[2]);
    glsl.draw(&dst);
 
    return dst;
}


texture_2d smooth_filter(const texture_2d& tfm, 
                         const texture_2d& img,
                         int type,
                         float sigma)
{
    texture_2d dst(tfm.clone_format());

    if (type == 3) {
        glsl_program glsl("lic_fs.glsl");
        glsl.use();
        glsl.bind_sampler("tfm", tfm);
        glsl.bind_sampler("img", img);
        glsl.set_uniform_1f("sigma", sigma);
        glsl.set_uniform_2f("img_size", (float)dst.get_width(), (float)dst.get_height());
        glsl.draw(&dst);
    } else {
        glsl_program gauss((type == 1)? "gauss3x3_fs.glsl" : "gauss5x5_fs.glsl");
        gauss.use();
        gauss.bind_sampler("img", img);
        gauss.set_uniform_2f("img_size", (float)dst.get_width(), (float)dst.get_height());
        gauss.draw(&dst);                  
    }
 
    return dst;
}


void GLView::process() {
    makeCurrent();
	if (m_image.isNull()) {
		m_dst = texture_2d();
		return;
	}

	int w = m_image.width();
	int h = m_image.height();

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
    glPushAttrib(GL_TEXTURE_BIT);

    texture_2d src(GL_RGB16F_ARB, m_image.width(), m_image.height(), GL_BGRA, GL_UNSIGNED_BYTE, m_image.bits());
    texture_2d lab = rgb2lab(src);
    texture_2d tfm = tangent_flow_map(src, sst_sigma);
    texture_2d bfe = (bf_ne > 0)? orientation_aligned_bilateral_filter(lab, tfm, bf_ne, bf_sigma_d, bf_sigma_r) : lab;
    texture_2d bfa = (bf_na > 0)? orientation_aligned_bilateral_filter(lab, tfm, bf_na, bf_sigma_d, bf_sigma_r) : lab;
    texture_2d edges = (fdog_type == 0)?
    fdog_filter(bfe, tfm, fdog_n, fdog_sigma_e, fdog_sigma_r, fdog_tau, fdog_sigma_m, fdog_phi) :
    dog_filter(bfe, fdog_n, fdog_sigma_e, fdog_sigma_r, fdog_tau, fdog_phi);
    texture_2d cq = color_quantization(bfa, cq_nbins, cq_phi_q, cq_filter);
    texture_2d cq_rgb = lab2rgb(cq);
    texture_2d ov = mix_filter(edges, cq_rgb, fdog_color);
    texture_2d result = fs_type? smooth_filter(tfm, ov, fs_type, fs_sigma) : ov;
    
    switch (preview) {
        case 0:
            m_dst = result;
            break;
		case 1:
			m_dst = src;
			break;
        case 2:
            m_dst = lic_filter(tfm, m_noise, 5.0);
            break;
        case 3:
            m_dst = lab2rgb(bfe);
            break;
        case 4:
            m_dst = lab2rgb(bfa);
            break;
        case 5:
            m_dst = edges;
            break;
        case 6:
            m_dst = cq_rgb;
            break;
    }

    glUseProgram(0);
    glPopAttrib();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


void GLView::updateAll() {
	process();
	updateGL();
}
