#include "stdafx.h"
#include "graphics.h"
#include "renderer.h"
#include "input_manager.h"

void __cdecl main()
{
    renderer::init();
    input_manager::init();

    nk_context* context = renderer::get_context();
    nk_font* font = renderer::get_font();

    while (true)
    {
        input_manager::pump_input(context);

        /* draw */
        struct nk_vec2 panel_padding = context->style.window.padding;
        struct nk_vec2 item_spacing = context->style.window.spacing;
        nk_style_item window_background = context->style.window.fixed_background;

        /* use the complete window space and set background */
        context->style.window.spacing = nk_vec2(0,0);
        context->style.window.padding = nk_vec2(0,0);
        context->style.window.fixed_background = nk_style_item_color(nk_rgb(250,250,250));

        /* create/update window and set position + size */
        nk_flags flags = 0;
        flags = flags & ~NK_WINDOW_DYNAMIC;
        nk_window_set_bounds(context, "Window", nk_rect(0, 0, graphics::getWidth(), graphics::getHeight()));
        nk_begin(context, "Window", nk_rect(0, 0, graphics::getWidth(), graphics::getHeight()), NK_WINDOW_NO_SCROLLBAR|flags);

        /* allocate the complete window space for drawing */
        struct nk_rect total_space;
        total_space = nk_window_get_content_region(context);
        nk_layout_row_dynamic(context, total_space.h, 1);
        nk_widget(&total_space, context);
        nk_command_buffer* painter = nk_window_get_canvas(context);

        nk_fill_rect(painter, nk_rect(15,15,210,210), 5, nk_rgb(247, 230, 154));
        nk_fill_rect(painter, nk_rect(20,20,200,200), 5, nk_rgb(188, 174, 118));
        nk_draw_text(painter, nk_rect(30, 30, 150, 20), "Text to draw", 12, &font->handle, nk_rgb(188,174,118), nk_rgb(0,0,0));
        nk_fill_rect(painter, nk_rect(250,20,100,100), 0, nk_rgb(0,0,255));
        nk_fill_circle(painter, nk_rect(20,250,100,100), nk_rgb(255,0,0));
        nk_fill_triangle(painter, 250, 250, 350, 250, 300, 350, nk_rgb(0,255,0));
        nk_fill_arc(painter, 300, 180, 50, 0, 3.141592654f * 3.0f / 4.0f, nk_rgb(255,255,0));

        float points[12];
        points[0] = 200; points[1] = 250;
        points[2] = 250; points[3] = 350;
        points[4] = 225; points[5] = 350;
        points[6] = 200; points[7] = 300;
        points[8] = 175; points[9] = 350;
        points[10] = 150; points[11] = 350;
        nk_fill_polygon(painter, points, 6, nk_rgb(0,0,0));

        nk_stroke_line(painter, 15, 10, 200, 10, 2.0f, nk_rgb(189,45,75));
        nk_stroke_rect(painter, nk_rect(370, 20, 100, 100), 10, 3, nk_rgb(0,0,255));
        nk_stroke_curve(painter, 380, 200, 405, 270, 455, 120, 480, 200, 2, nk_rgb(0,150,220));
        nk_stroke_circle(painter, nk_rect(20, 370, 100, 100), 5, nk_rgb(0,255,120));
        nk_stroke_triangle(painter, 370, 250, 470, 250, 420, 350, 6, nk_rgb(255,0,143));

        if (input_manager::has_mouse(-1))
        {
            MouseState mouseState;
            memset(&mouseState, 0, sizeof(mouseState));
            if (input_manager::try_get_mouse_state(-1, &mouseState))
            {
                renderer::mouse_pointer(mouseState.x, mouseState.y);
            }
        }

        nk_end(context);

        context->style.window.spacing = panel_padding;
        context->style.window.padding = item_spacing;
        context->style.window.fixed_background = window_background;

        renderer::render(0xff333333);
    }
}