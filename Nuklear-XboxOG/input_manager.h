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

typedef struct RemoteState
{
    bool buttons[44];
} RemoteState;

typedef enum REMOTE_BUTTON
{
    REMOTE_BUTTON_DISPLAY = 0,
    REMOTE_BUTTON_REVERSE = 1,
    REMOTE_BUTTON_PLAY = 2,
    REMOTE_BUTTON_FORWARD = 3,
    REMOTE_BUTTON_SKIP_MINUS = 4,
    REMOTE_BUTTON_STOP = 5,
    REMOTE_BUTTON_PAUSE = 6,
    REMOTE_BUTTON_SKIP_PLUS = 7,
    REMOTE_BUTTON_TITLE = 8,
    REMOTE_BUTTON_INFO = 9,
    REMOTE_BUTTON_UP = 10,
    REMOTE_BUTTON_DOWN = 11,
    REMOTE_BUTTON_LEFT = 12,
    REMOTE_BUTTON_RIGHT = 13,
    REMOTE_BUTTON_SELECT = 14,
    REMOTE_BUTTON_MENU = 15,
    REMOTE_BUTTON_BACK = 16,
    REMOTE_BUTTON_1 = 17,
    REMOTE_BUTTON_2 = 18,
    REMOTE_BUTTON_3 = 19,
    REMOTE_BUTTON_4 = 20,
    REMOTE_BUTTON_5 = 21,
    REMOTE_BUTTON_6 = 22,
    REMOTE_BUTTON_7 = 23,
    REMOTE_BUTTON_8 = 24,
    REMOTE_BUTTON_9 = 25,
    REMOTE_BUTTON_0 = 26,
    REMOTE_BUTTON_MCE_POWER = 27,
    REMOTE_BUTTON_MCE_MY_TV = 28,
    REMOTE_BUTTON_MCE_MY_MUSIC = 29,
    REMOTE_BUTTON_MCE_MY_PICTURES = 30,
    REMOTE_BUTTON_MCE_MY_VIDEOS = 31,
    REMOTE_BUTTON_MCE_RECORD = 32,
    REMOTE_BUTTON_MCE_START = 33,
    REMOTE_BUTTON_MCE_VOLUME_PLUS = 34,
    REMOTE_BUTTON_MCE_VOLUME_MINUS = 35,
    REMOTE_BUTTON_MCE_CHANNEL_PLUS = 36,
    REMOTE_BUTTON_MCE_CHANNEL_MINUS = 37,
    REMOTE_BUTTON_MCE_MUTE = 38,
    REMOTE_BUTTON_MCE_RECORDED_TV = 39,
    REMOTE_BUTTON_MCE_LIVE_TV = 40,
    REMOTE_BUTTON_MCE_STAR = 41,
    REMOTE_BUTTON_MCE_HASH = 42,
    REMOTE_BUTTON_MCE_CLEAR = 43
} REMOTE_BUTTON;

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
    static void process_remote();
    static void process_mouse();
    static void process_keyboard();
    static bool controller_pressed(CONTROLLER_BUTTON button, int port);
    static bool remote_pressed(REMOTE_BUTTON button, int port);
    static bool mouse_pressed(MOUSE_BUTTON button, int port);
    static bool try_get_controller_state(int port, ControllerState* controllerState);
    static bool try_get_remote_state(int port, RemoteState* remoteState);
    static bool try_get_mouse_state(int port, MouseState* mouseState);
    static bool try_get_keyboard_state(int port, KeyboardState* keyboardState);
    static bool has_controller(int port);
    static bool has_remote(int port);
    static bool has_mouse(int port);
    static void pump_input(nk_context *context);
    static MousePosition get_mouse_position();
};
