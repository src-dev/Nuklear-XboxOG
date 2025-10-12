#pragma once

#include <xtl.h>
#include "stdint.h"

#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"

class renderer
{
public:
    static bool init();
    static void render(uint32_t background_color);
    static void present();
    static nk_context* get_context();
    static nk_font* get_font();
    static void mouse_pointer();
};
