#include "focus.h"
#include "..\graphics.h"
#include "..\renderer.h"
#include "..\debug.h"
#include "..\input_manager.h"

#include <stdio.h>

#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "..\nuklear.h"

struct FocusManager {
    int count;
    int focused;
    struct nk_context *ctx;
};

static struct FocusManager focus_mgr = {0};

static void fm_begin(struct nk_context *ctx)
{
    focus_mgr.count = 0;
    focus_mgr.ctx = ctx;
}

static void fm_end(void)
{
    struct nk_input *in = &focus_mgr.ctx->input;
    if (focus_mgr.count == 0)
        return;

    if (nk_input_is_key_pressed(in, NK_KEY_TAB)) {
        if (in->keyboard.keys[NK_KEY_SHIFT].down)
            focus_mgr.focused = (focus_mgr.focused - 1 + focus_mgr.count) % focus_mgr.count;
        else
            focus_mgr.focused = (focus_mgr.focused + 1) % focus_mgr.count;
    }

    if (input_manager::controller_pressed(CONTROLLER_Y_BUTTON, -1))
    {
        focus_mgr.focused = (focus_mgr.focused - 1 + focus_mgr.count) % focus_mgr.count;
    }
    else if (input_manager::controller_pressed(CONTROLLER_X_BUTTON, -1))
    {
        focus_mgr.focused = (focus_mgr.focused + 1) % focus_mgr.count;
    }
}

static nk_bool fm_enter_pressed(void)
{
    struct nk_input *in = &focus_mgr.ctx->input;
    bool result = nk_input_is_key_pressed(in, NK_KEY_ENTER);
    result |= input_manager::controller_pressed(CONTROLLER_A_BUTTON, -1);
    return result;
}

static int fm_register(void)
{
    return focus_mgr.count++;
}

static nk_bool fm_is_focused(int index)
{
    return (index == focus_mgr.focused);
}

static void fm_push_focus_style(struct nk_context *ctx, nk_bool focused)
{
    if (focused)
    {
        nk_style_push_color(ctx, &ctx->style.button.border_color, nk_rgb(255, 255, 0));
    }
}

static void fm_pop_focus_style(struct nk_context *ctx, nk_bool focused)
{
    if (focused)
    {
        nk_style_pop_color(ctx);
    }
}

static void fm_push_checkbox_focus_style(struct nk_context *ctx, nk_bool focused)
{
    if (focused) {
        struct nk_style_toggle *s = &ctx->style.checkbox;
        nk_style_push_color(ctx, &s->normal.data.color, nk_rgb(255, 255, 0)); 
        nk_style_push_color(ctx, &s->hover.data.color, nk_rgb(255, 255, 0));
        nk_style_push_color(ctx, &s->active.data.color, nk_rgb(255, 255, 0));
    }
}

static void fm_pop_checkbox_focus_style(struct nk_context *ctx, nk_bool focused)
{
    if (focused) {
        nk_style_pop_color(ctx); 
        nk_style_pop_color(ctx); 
        nk_style_pop_color(ctx); 
    }
}

void focus::render()
{
    nk_context* ctx = renderer::get_context();

    if (nk_begin(ctx, "Focus", nk_rect(30, 300, 200, 250), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_CLOSABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {
        static char text[64] = "Type here";
        static int check = nk_false;
        static int button1_pressed = 0;
        static int button2_pressed = 0;

        fm_begin(ctx);

        /* ---- Title Label ---- */
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_label(ctx, "This is a Focus Test Example", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx, 30, 1);

        /* ---- Button 1 ---- */
        int id0 = fm_register();
        fm_push_focus_style(ctx, fm_is_focused(id0));
        if (nk_button_label(ctx, "Button 1") || (fm_is_focused(id0) && fm_enter_pressed()))
            button1_pressed = 1;
        fm_pop_focus_style(ctx, fm_is_focused(id0));

        /* ---- Edit Field ---- */
        int id1 = fm_register();
        nk_flags edit_flags = NK_EDIT_FIELD;
        if (fm_is_focused(id1))
            nk_edit_focus(ctx, edit_flags);
        else
            nk_edit_unfocus(ctx);
        nk_edit_string_zero_terminated(ctx, edit_flags, text, sizeof(text), nk_filter_default);

        /* ---- Checkbox ---- */
        int id2 = fm_register();
        fm_push_checkbox_focus_style(ctx, fm_is_focused(id2));
        if (fm_is_focused(id2) && fm_enter_pressed())
            check = !check;
        nk_checkbox_label(ctx, "Enable feature", &check);
        fm_pop_checkbox_focus_style(ctx, fm_is_focused(id2));

        /* ---- Button 2 ---- */
        int id3 = fm_register();
        fm_push_focus_style(ctx, fm_is_focused(id3));
        if (nk_button_label(ctx, "Button 2") || (fm_is_focused(id3) && fm_enter_pressed()))
            button2_pressed = 1;
        fm_pop_focus_style(ctx, fm_is_focused(id3));

        fm_end();

        if (button1_pressed) {
            nk_label(ctx, "Button 1 pressed!", NK_TEXT_LEFT);
            button1_pressed = 0;
        }
        if (button2_pressed) {
            nk_label(ctx, "Button 2 pressed!", NK_TEXT_LEFT);
            button2_pressed = 0;
        }
    }
    nk_end(ctx);
}