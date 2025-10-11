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
    MOUSE_EXTRA_BUTTON1 = 3, 
    MOUSE_EXTRA_BUTTON2 = 4 
} MOUSE_BUTTON;

typedef  struct KeyboardState
{
  bool key_down;
  char ascii;
  char virtual_key;
  bool button[6];
} KeyboardState;

typedef enum KEYBOARD_BUTTON 
{ 
    KEYBOARD_CTRL_BUTTON = 0,
    KEYBOARD_SHIFT_BUTTON = 0,  
    KEYBOARD_ALT_BUTTON = 2,
    KEYBOARD_CAPSLOCK_BUTTON = 3,
    KEYBOARD_NUMLOCK_BUTTON = 4,
    KEYBOARD_SCROLLLOCK_BUTTON = 5
} KEYBOARD_BUTTON;

class input_manager
{
public:
    static void init();
    static void process_mouse();
    static void process_keyboard();
    static bool mouse_pressed(MOUSE_BUTTON button, int port);
    static bool try_get_mouse_state(int port, MouseState* mouseState);
    static bool try_get_keyboard_state(int port, KeyboardState* keyboardState);
    static void pump_input(nk_context *context);
};
