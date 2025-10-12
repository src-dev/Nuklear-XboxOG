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

typedef struct MousePosition
{
  float x;
  float y;
} MousePosition;

typedef struct ControllerState
{
    float thumb_left_dx;
    float thumb_left_dy;
    float thumb_right_dx;
    float thumb_right_dy;
    bool buttons[16];
} ControllerState;

typedef enum CONTROLLER_BUTTON 
{ 
    CONTROLLER_A_BUTTON = 0, 
    CONTROLLER_B_BUTTON = 1, 
    CONTROLLER_X_BUTTON = 2, 
    CONTROLLER_Y_BUTTON = 3, 
    CONTROLLER_BLACK_BUTTON = 4,
    CONTROLLER_WHITE_BUTTON = 5,
    CONTROLLER_LTRIGGER_BUTTON = 6,
    CONTROLLER_RTRIGGER_BUTTON = 7,
    CONTROLLER_DPAD_UP_BUTTON = 8,
    CONTROLLER_DPAD_DOWN_BUTTON = 9,
    CONTROLLER_DPAD_LEFT_BUTTON = 10,
    CONTROLLER_DPAD_RIGHT_BUTTON = 11,
    CONTROLLER_START_BUTTON = 12,
    CONTROLLER_BACK_BUTTON = 13,
    CONTROLLER_LTHUMB_BUTTON = 14,
    CONTROLLER_RTHUMB_BUTTON = 15,
} CONTROLLER_BUTTON;

typedef struct MouseState
{
  float dx;
  float dy;
  float dz;
  bool buttons[5]; 
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
    static void process_controller();
    static void process_mouse();
    static void process_keyboard();
    static bool controller_pressed(CONTROLLER_BUTTON button, int port);
    static bool mouse_pressed(MOUSE_BUTTON button, int port);
    static bool try_get_controller_state(int port, ControllerState* controllerState);
    static bool try_get_mouse_state(int port, MouseState* mouseState);
    static bool has_controller(int port);
    static bool has_mouse(int port);
    static bool try_get_keyboard_state(int port, KeyboardState* keyboardState);
    static void pump_input(nk_context *context);
    static MousePosition get_mouse_position();
};
