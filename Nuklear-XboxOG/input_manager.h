#pragma once

#include <xtl.h>
#include "stdint.h"

#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"

typedef  struct MouseState
{
  int x;
  int y;
  int dx;
  int dy;
  int dz;
  bool button[5]; 
} MouseState;

typedef enum MOUSE_BUTTON 
{ 
    MOUSE_LEFT_BUTTON = 0, 
    MOUSE_RIGHT_BUTTON = 1, 
    MOUSE_MIDDLE_BUTTON = 2, 
    MOUSE_EXTRA_BUTTON1 = 4, 
    MOUSE_EXTRA_BUTTON2 = 8 
} MOUSE_BUTTON;

class input_manager
{
public:
    static void process();
    static bool mouse_pressed(MOUSE_BUTTON button, int port);
    static bool try_get_mouse_state(int port, MouseState* mouseState);
};
