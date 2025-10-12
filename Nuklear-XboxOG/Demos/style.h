#pragma once

#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "..\nuklear.h"

enum theme_style {
    THEME_BLACK,
    THEME_WHITE,
    THEME_RED,
    THEME_BLUE,
    THEME_DARK,
    THEME_DRACULA,
    THEME_CATPPUCCIN_LATTE,
    THEME_CATPPUCCIN_FRAPPE,
    THEME_CATPPUCCIN_MACCHIATO,
    THEME_CATPPUCCIN_MOCHA
};

class style
{
public:
    static void set(struct nk_context *ctx, enum theme theme);
};
