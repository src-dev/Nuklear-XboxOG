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

typedef struct nk_vertex 
{
    float position[3];
    nk_byte col[4];
    float uv[2];
} nk_vertex;

class renderer
{
public:
    static bool init();
    static void begin_render(uint32_t background_color);
    static void render();
    static void end_render();
    static nk_context* get_context();
    static nk_font* get_font();
    static void mouse_pointer();
};
