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
#include "glsl_program.h"
#include <string>
#include <cstdio>
#ifdef _MSC_VER
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif


namespace {

    typedef std::tr1::unordered_map<std::string,int> program_map;
    static program_map *g_map = 0;

}
 

void glsl_program::cleanup() {
}


glsl_program::glsl_program( const char *id, bool create ) : m_id (0) {
     if (!g_map) {
         g_map = new program_map;
     }
     program_map::iterator i = g_map->find(id);
     if (create) {
         assert(i == g_map->end());
         m_id = glCreateProgram();
         (*g_map)[id] = m_id;
     } else {
         assert(i != g_map->end());
         if (i != g_map->end()) {
             m_id = i->second;
         }
     }
}


glsl_program::~glsl_program() {
}


void glsl_program::use() {
    glUseProgram(m_id);
}


bool glsl_program::has_uniform( const char* name ) const {
    return glGetUniformLocation(m_id, name) != -1;
}


void glsl_program::set_uniform_1i( const char* name, int value ) {
    GLint location = glGetUniformLocation(m_id, name);
    if (location >= 0) {
        glUniform1i(location, value);
        //assert(glGetError() == GL_NO_ERROR);
    }
}


void glsl_program::set_uniform_1f( const char* name, float value ) {
    GLint location = glGetUniformLocation(m_id, name);
    if (location >= 0) {
        glUniform1f(location, value);
        //assert(glGetError() == GL_NO_ERROR);
    }
}


void glsl_program::set_uniform_2f( const char* name, float x, float y ) {
    GLint location = glGetUniformLocation(m_id, name);
    if (location >= 0) {
        glUniform2f(location, x, y);
        //assert(glGetError() == GL_NO_ERROR);
    }
}


void glsl_program::set_uniform_3f( const char* name, float x, float y, float z ) {
    GLint location = glGetUniformLocation(m_id, name);
    if (location >= 0) {
        glUniform3f(location, x, y, z);
        //assert(glGetError() == GL_NO_ERROR);
    }
}


void glsl_program::bind_sampler( const char* name, const texture_2d& tex, GLenum filter /*= GL_NEAREST*/ ) {
    GLint location = glGetUniformLocation(m_id, name);
    if (location >= 0) {
        GLint unit;
        glGetUniformiv(m_id, location, &unit);
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, tex.get_id());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    }
}


void glsl_program::draw( texture_2d *ca0, texture_2d *ca1, texture_2d *ca2, texture_2d *ca3) {
    texture_2d *ca[4];
    GLenum db[4];
    int n = 0;
    if (ca0) ca[n++] = ca0;
    if (ca1) ca[n++] = ca1;
    if (ca2) ca[n++] = ca2;
    if (ca3) ca[n++] = ca3;
    for (int i = 0; i < n; ++i) {
        db[i] = GL_COLOR_ATTACHMENT0_EXT + i;
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, db[i], GL_TEXTURE_2D, ca[i]->get_id(), 0);
    }
    glDrawBuffers(n, db);
    glRectf(-1, -1, 1, 1);
}


bool glsl_program::attach_shader( GLenum type, const char* source ) {
    GLuint sid = glCreateShader(type);
    glShaderSource(sid, 1, &source, NULL);
    glCompileShader(sid);

    GLint status;
    glGetShaderiv(sid, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint len;
        glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &len);
        if (len > 0) {
            char *log = new char[len];
            glGetShaderInfoLog(sid, len, NULL, log);
            #ifdef WIN32
            OutputDebugStringA(log);
            #endif
            fprintf(stderr, "%s\n", log);
            delete[] log;
        }
        return false;
    }

    return attach_shader(sid);
}


bool glsl_program::attach_shader( GLuint sid ) {
    glAttachShader(m_id, sid);
    bool result = glGetError() == GL_NO_ERROR;
    assert(result);
    return result;
}


bool glsl_program::link() {
    glLinkProgram(m_id);
    
    GLint status;
    glGetProgramiv(m_id, GL_LINK_STATUS, &status);
    if (status) {
        glValidateProgram(m_id);
    }

    GLint len = 0;
    glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        char *log = new char[len];
        glGetProgramInfoLog(m_id, len, NULL, log);
        #ifdef WIN32
        OutputDebugStringA(log);
        #endif
        fprintf(stderr, "%s\n", log);
        delete[] log;
    }

    GLint current_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
    glUseProgram(m_id);
    if (status != 0) {
        GLint num_uniforms;
        glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &num_uniforms);
        GLint buf_size;
        glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &buf_size);
        GLchar *buf_name = new GLchar[buf_size];
        int nunit = 0;
        for (int i = 0; i < num_uniforms; ++i) {
            GLenum type;
            GLint size;
            glGetActiveUniform(m_id, i, buf_size, NULL, &size, &type, buf_name);
            if ((size == 1) && (type == GL_SAMPLER_2D)) {
                glUniform1i(glGetUniformLocation(m_id, buf_name), nunit);
                ++nunit;
            }
        }
        delete[] buf_name;
    }
    glUseProgram(current_program);
    return true;
}
