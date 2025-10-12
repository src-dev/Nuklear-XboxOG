#include "controls.h"
#include "..\graphics.h"
#include "..\renderer.h"
#include "..\debug.h"

#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "..\nuklear.h"

void controls::render()
{
    nk_context* context = renderer::get_context();

    if (nk_begin(context, "Controls", nk_rect(50, 50, 230, 250), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_CLOSABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {
        enum {EASY, HARD};
        static int op = EASY;
        static int property = 20;    
        static struct nk_colorf bg = { 0.10f, 0.18f, 0.24f, 1.0f }; 

        nk_layout_row_static(context, 30, 80, 1);
        if (nk_button_label(context, "button"))
        {
            debug::print("button pressed\n");
        }
        nk_layout_row_dynamic(context, 30, 2);
        if (nk_option_label(context, "easy", op == EASY))
        {
            op = EASY;
        }
        if (nk_option_label(context, "hard", op == HARD)) 
        {
            op = HARD;
        }
        nk_layout_row_dynamic(context, 22, 1);
        nk_property_int(context, "Compression:", 0, &property, 100, 10, 1);

        nk_layout_row_dynamic(context, 20, 1);
        nk_label(context, "background:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(context, 25, 1);
        if (nk_combo_begin_color(context, nk_rgb_cf(bg), nk_vec2(nk_widget_width(context),400))) 
        {
            nk_layout_row_dynamic(context, 120, 1);
            bg = nk_color_picker(context, bg, NK_RGBA);
            nk_layout_row_dynamic(context, 25, 1);
            bg.r = nk_propertyf(context, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
            bg.g = nk_propertyf(context, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
            bg.b = nk_propertyf(context, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
            bg.a = nk_propertyf(context, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
            nk_combo_end(context);
        }
    }
    nk_end(context);
}