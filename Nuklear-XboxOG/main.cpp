#include "stdafx.h"
#include "graphics.h"
#include "nk_render.h"
#include "input_manager.h"

static void pump_input(struct nk_context *context)
{
    input_manager::process_mouse();
    input_manager::process_keyboard();

    nk_input_begin(context);

    KeyboardState keyboardState;
    memset(&keyboardState, 0, sizeof(keyboardState));
    if (input_manager::try_get_keyboard_state(-1, &keyboardState))
    {
        nk_input_key(context, NK_KEY_DEL, keyboardState.virtual_key == VK_DELETE && keyboardState.key_down);
        nk_input_key(context, NK_KEY_ENTER, keyboardState.virtual_key == VK_RETURN && keyboardState.key_down);
        nk_input_key(context, NK_KEY_TAB, keyboardState.virtual_key == VK_TAB && keyboardState.key_down);
        nk_input_key(context, NK_KEY_BACKSPACE, keyboardState.virtual_key == VK_BACK && keyboardState.key_down);
        nk_input_key(context, NK_KEY_LEFT, keyboardState.virtual_key == VK_LEFT && keyboardState.key_down);
        nk_input_key(context, NK_KEY_RIGHT, keyboardState.virtual_key == VK_RIGHT && keyboardState.key_down);
        nk_input_key(context, NK_KEY_UP, keyboardState.virtual_key == VK_UP && keyboardState.key_down);
        nk_input_key(context, NK_KEY_DOWN, keyboardState.virtual_key == VK_DOWN && keyboardState.key_down);

        if (keyboardState.button[KEYBOARD_CTRL_BUTTON] && keyboardState.key_down) 
        {
            nk_input_key(context, NK_KEY_COPY, keyboardState.ascii == 'c' || keyboardState.ascii == 'C');
            nk_input_key(context, NK_KEY_PASTE, keyboardState.ascii == 'p' || keyboardState.ascii == 'P');
            nk_input_key(context, NK_KEY_CUT, keyboardState.ascii == 'x' || keyboardState.ascii == 'X');
        } 
        else 
        {
            nk_input_key(context, NK_KEY_COPY, 0);
            nk_input_key(context, NK_KEY_PASTE, 0);
            nk_input_key(context, NK_KEY_CUT, 0);
        }

        if (keyboardState.ascii >= 0x20 && keyboardState.ascii <= 0x7e && keyboardState.key_down == true)
        {
            nk_input_char(context, keyboardState.ascii);
        }
    }

    MouseState mouseState;
    memset(&mouseState, 0, sizeof(mouseState));
    if (input_manager::try_get_mouse_state(-1, &mouseState))
    {
        nk_input_motion(context, mouseState.x, mouseState.y);
        nk_input_button(context, NK_BUTTON_LEFT, mouseState.x, mouseState.y, mouseState.button[MOUSE_LEFT_BUTTON]);
        nk_input_button(context, NK_BUTTON_MIDDLE, mouseState.x, mouseState.y, mouseState.button[MOUSE_MIDDLE_BUTTON]);
        nk_input_button(context, NK_BUTTON_RIGHT, mouseState.x, mouseState.y, mouseState.button[MOUSE_RIGHT_BUTTON]);
    }
    nk_input_end(context);
}

struct nk_canvas {
    struct nk_command_buffer *painter;
    struct nk_vec2 item_spacing;
    struct nk_vec2 panel_padding;
    struct nk_style_item window_background;
};

static void
canvas_begin(struct nk_context *ctx, struct nk_canvas *canvas, nk_flags flags,
    int x, int y, int width, int height, struct nk_color background_color)
{
    /* save style properties which will be overwritten */
    canvas->panel_padding = ctx->style.window.padding;
    canvas->item_spacing = ctx->style.window.spacing;
    canvas->window_background = ctx->style.window.fixed_background;

    /* use the complete window space and set background */
    ctx->style.window.spacing = nk_vec2(0,0);
    ctx->style.window.padding = nk_vec2(0,0);
    ctx->style.window.fixed_background = nk_style_item_color(background_color);

    /* create/update window and set position + size */
    flags = flags & ~NK_WINDOW_DYNAMIC;
    nk_window_set_bounds(ctx, "Window", nk_rect(x, y, width, height));
    nk_begin(ctx, "Window", nk_rect(x, y, width, height), NK_WINDOW_NO_SCROLLBAR|flags);

    /* allocate the complete window space for drawing */
    {struct nk_rect total_space;
    total_space = nk_window_get_content_region(ctx);
    nk_layout_row_dynamic(ctx, total_space.h, 1);
    nk_widget(&total_space, ctx);
    canvas->painter = nk_window_get_canvas(ctx);}
}

static void
canvas_end(struct nk_context *ctx, struct nk_canvas *canvas)
{
    nk_end(ctx);
    ctx->style.window.spacing = canvas->panel_padding;
    ctx->style.window.padding = canvas->item_spacing;
    ctx->style.window.fixed_background = canvas->window_background;
}

void __cdecl main()
{
    nk_render::init();
    input_manager::init();

    int width = graphics::getWidth();
    int height = graphics::getHeight();

    nk_context* context = nk_render::get_context();
    nk_font* font = nk_render::get_font();

    while (true)
    {
        pump_input(context);

        /* draw */
        nk_canvas canvas;
        canvas_begin(context, &canvas, 0, 0, 0, width, height, nk_rgb(250,250,250));
        {
   

            nk_fill_rect(canvas.painter, nk_rect(15,15,210,210), 5, nk_rgb(247, 230, 154));
            nk_fill_rect(canvas.painter, nk_rect(20,20,200,200), 5, nk_rgb(188, 174, 118));
            nk_draw_text(canvas.painter, nk_rect(30, 30, 150, 20), "Text to draw", 12, &font->handle, nk_rgb(188,174,118), nk_rgb(0,0,0));
            nk_fill_rect(canvas.painter, nk_rect(250,20,100,100), 0, nk_rgb(0,0,255));
            nk_fill_circle(canvas.painter, nk_rect(20,250,100,100), nk_rgb(255,0,0));
            nk_fill_triangle(canvas.painter, 250, 250, 350, 250, 300, 350, nk_rgb(0,255,0));
            nk_fill_arc(canvas.painter, 300, 180, 50, 0, 3.141592654f * 3.0f / 4.0f, nk_rgb(255,255,0));

            {float points[12];
            points[0] = 200; points[1] = 250;
            points[2] = 250; points[3] = 350;
            points[4] = 225; points[5] = 350;
            points[6] = 200; points[7] = 300;
            points[8] = 175; points[9] = 350;
            points[10] = 150; points[11] = 350;
            nk_fill_polygon(canvas.painter, points, 6, nk_rgb(0,0,0));}

            nk_stroke_line(canvas.painter, 15, 10, 200, 10, 2.0f, nk_rgb(189,45,75));
            nk_stroke_rect(canvas.painter, nk_rect(370, 20, 100, 100), 10, 3, nk_rgb(0,0,255));
            nk_stroke_curve(canvas.painter, 380, 200, 405, 270, 455, 120, 480, 200, 2, nk_rgb(0,150,220));
            nk_stroke_circle(canvas.painter, nk_rect(20, 370, 100, 100), 5, nk_rgb(0,255,120));
            nk_stroke_triangle(canvas.painter, 370, 250, 470, 250, 420, 350, 6, nk_rgb(255,0,143));

            // quick n dirty mouse pointer
            MouseState mouseState;
            memset(&mouseState, 0, sizeof(mouseState));
            if (input_manager::try_get_mouse_state(-1, &mouseState))
            {
                nk_fill_rect(canvas.painter, nk_rect(mouseState.x - 5, mouseState.y  - 5, 10,10), 5, nk_rgb(255, 0, 255));
            }
        }
        canvas_end(context, &canvas);

        nk_render::render(0xff333333);
    }
}
