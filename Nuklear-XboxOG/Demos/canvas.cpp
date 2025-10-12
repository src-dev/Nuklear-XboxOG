#include "canvas.h"
#include "..\graphics.h"
#include "..\renderer.h"

#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "..\nuklear.h"

void canvas::render()
{
    nk_context* context = renderer::get_context();
    nk_font* font = renderer::get_font();

    /* draw */
    struct nk_vec2 panel_padding = context->style.window.padding;
    struct nk_vec2 item_spacing = context->style.window.spacing;
    nk_style_item window_background = context->style.window.fixed_background;

    if (nk_begin(context, "Canvas", nk_rect(10, 10, (float)500, (float)400), NK_WINDOW_TITLE|NK_WINDOW_BORDER|NK_WINDOW_SCALABLE|NK_WINDOW_MOVABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_SCROLL_AUTO_HIDE))
    {
        /* allocate the complete window space for drawing */
        struct nk_rect total_space;
        total_space = nk_window_get_content_region(context);
        nk_layout_row_dynamic(context, total_space.h, 1);
        nk_widget(&total_space, context);
        nk_command_buffer* painter = nk_window_get_canvas(context);

        float x = painter->clip.x;
        float y = painter->clip.y;
        nk_fill_rect(painter, nk_rect(x + 15, y + 15, 210, 210), 5, nk_rgb(247, 230, 154));
        nk_fill_rect(painter, nk_rect(x + 20, y + 20, 200, 200), 5, nk_rgb(188, 174, 118));
        nk_draw_text(painter, nk_rect(x + 30, y + 30, 150, 20), "Text to draw", 12, &font->handle, nk_rgb(188,174,118), nk_rgb(0,0,0));
        nk_fill_rect(painter, nk_rect(x + 250, y + 20, 100, 100), 0, nk_rgb(0,0,255));
        nk_fill_circle(painter, nk_rect(x + 20, y + 250, 100, 100), nk_rgb(255,0,0));
        nk_fill_triangle(painter, x + 250, y + 250, x + 350, y + 250, x + 300, y + 350, nk_rgb(0,255,0));
        nk_fill_arc(painter, x + 300, y + 420, 50, 0, 3.141592654f * 3.0f / 4.0f, nk_rgb(255,255,0));

        float points1[12];
        points1[0]  = x + 200; points1[1]  = y + 250;
        points1[2]  = x + 250; points1[3]  = y + 350;
        points1[4]  = x + 225; points1[5]  = y + 350;
        points1[6]  = x + 200; points1[7]  = y + 300;
        points1[8]  = x + 175; points1[9]  = y + 350;
        points1[10] = x + 150; points1[11] = y + 350;
        nk_fill_polygon(painter, points1, 6, nk_rgb(0,0,0));

        float points2[12];
        points2[0]  = x + 200; points2[1]  = y + 370;
        points2[2]  = x + 250; points2[3]  = y + 470;
        points2[4]  = x + 225; points2[5]  = y + 470;
        points2[6]  = x + 200; points2[7]  = y + 420;
        points2[8]  = x + 175; points2[9]  = y + 470;
        points2[10] = x + 150; points2[11] = y + 470;
        nk_stroke_polygon(painter, points2, 6, 4, nk_rgb(0,0,0));

        float points3[8];
        points3[0]  = x + 250; points3[1]  = y + 200;
        points3[2]  = x + 275; points3[3]  = y + 220;
        points3[4]  = x + 325; points3[5]  = y + 170;
        points3[6]  = x + 350; points3[7]  = y + 200;
        nk_stroke_polyline(painter, points3, 4, 2, nk_rgb(255,128,0));

        nk_stroke_line(painter, x + 15, y + 10, x + 200, y + 10, 2.0f, nk_rgb(189,45,75));
        nk_stroke_rect(painter, nk_rect(x + 370, y + 20, 100, 100), 10, 3, nk_rgb(0,0,255));
        nk_stroke_curve(painter, x + 380, y + 200, x + 405, y + 270, x + 455, y + 120, x + 480, y + 200, 2, nk_rgb(0,150,220));
        nk_stroke_circle(painter, nk_rect(x + 20, y + 370, 100, 100), 5, nk_rgb(0,255,120));
        nk_stroke_triangle(painter, x + 370, y + 250, x + 470, y + 250, x + 420, y + 350, 6, nk_rgb(255,0,143));
        nk_stroke_arc(painter, x + 420, y + 420, 50, 0, 3.141592654f * 3.0f / 4.0f, 5, nk_rgb(0,255,255));
    }
    nk_end(context);
}