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
#include "texture_2d.h"
#ifdef _MSC_VER
#include <unordered_set>
#else
#include <tr1/unordered_set>
#endif


namespace {
    struct buffer_equal_to {
        bool operator()(const texture_2d::buffer_t* x, const texture_2d::buffer_t* y) const {
            return (x->w == y->w) && (x->h == y->h) && (x->format == y->format);
        }
    };

    struct buffer_hash {
        static size_t hash_combine(size_t seed, size_t v) {
            return seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        std::size_t operator()(const texture_2d::buffer_t *b) const {
            size_t seed = 0;
            hash_combine(seed, b->w);
            hash_combine(seed, b->h);
            hash_combine(seed, b->format);
            return seed;
        }
    };

    typedef std::tr1::unordered_multiset<texture_2d::buffer_t*, buffer_hash, buffer_equal_to> cache_set;
    static cache_set *g_cache = 0;
}


texture_2d::buffer_t* texture_2d::get_buffer( GLint format, int w, int h ) {
    if (!g_cache) {
        g_cache = new cache_set;
    }

    buffer_t bdata;
    bdata.w = w;
    bdata.h = h;
    bdata.format = format;
    cache_set::iterator i = g_cache->find(&bdata);

    buffer_t *b;
    if (i != g_cache->end()) {
        b = *i;
        g_cache->erase(i);
    } else {
        b = new buffer_t;
    }
    b->ref_count = 1;
    return b;
}


void texture_2d::put_buffer( buffer_t *b) {
    assert(g_cache);
    if (!g_cache) return;
    g_cache->insert(b);
}


void texture_2d::cleanup() {
    if (g_cache) {
        for (cache_set::iterator i = g_cache->begin(); i != g_cache->end(); ++i) {
            buffer_t *b = *i;
            glGenTextures(1, &b->id);
            delete b;
        }
        delete g_cache;
        g_cache = 0;
    }    
}
