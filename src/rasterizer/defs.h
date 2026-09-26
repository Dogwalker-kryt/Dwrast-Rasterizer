#pragma once

#include "Framebuffer/Framebuffer.hpp"

#define WIDTH 1000
#define HEIGTH 500

using FB = dwrast::Framebuffer<WIDTH, HEIGTH>;

template<uint64_t width, uint64_t heigth>
using FB2 = dwrast::Framebuffer<width, heigth>;

#define INT_MINRES(a, b) \
    (((X) < (Y)) ? (X) : (Y))

#