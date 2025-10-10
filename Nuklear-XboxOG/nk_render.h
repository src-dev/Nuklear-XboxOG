#pragma once

#include <xtl.h>
#include "stdint.h"

#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"

class nk_render
{
public:
    static bool init();
    static void render(uint32_t background_color);
    static nk_context* get_context();
    static nk_font* get_font();
};
