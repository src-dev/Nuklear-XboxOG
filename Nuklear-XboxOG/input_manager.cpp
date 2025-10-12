#include "input_manager.h"
#include "graphics.h"
#include "math.h"
#include "debug.h"

namespace
{
	bool mInitialized = false;

    float velocity_x;
    float velocity_y;
    MousePosition mMousePosition;

	HANDLE mControllerHandles[XGetPortCount()];
    DWORD mControllerLastPacketNumber[XGetPortCount()];
    ControllerState mControllerStatesCurrent[XGetPortCount()];
    ControllerState mControllerStatesPrevious[XGetPortCount()];

	HANDLE mMouseHandles[XGetPortCount()];
    DWORD mMouseLastPacketNumber[XGetPortCount()];
    MouseState mMouseStatesCurrent[XGetPortCount()];
    MouseState mMouseStatesPrevious[XGetPortCount()];

    HANDLE mKeyboardHandles[XGetPortCount()];
    KeyboardState mKeyboardState;
}

void input_manager::init()
{
    XInitDevices(0, 0);

    velocity_x = 0;
    velocity_y = 0;
    memset(&mMousePosition, 0, sizeof(mMousePosition));

    memset(mControllerHandles, 0, sizeof(mControllerHandles));
    memset(mControllerLastPacketNumber, 0, sizeof(mControllerLastPacketNumber));
    memset(mControllerStatesCurrent, 0, sizeof(mControllerStatesCurrent));
    memset(mControllerStatesPrevious, 0, sizeof(mControllerStatesPrevious));

    memset(mMouseHandles, 0, sizeof(mMouseHandles));
    memset(mMouseLastPacketNumber, 0, sizeof(mMouseLastPacketNumber));
    memset(mMouseStatesCurrent, 0, sizeof(mMouseStatesCurrent));
    memset(mMouseStatesPrevious, 0, sizeof(mMouseStatesPrevious));

    memset(mKeyboardHandles, 0, sizeof(mKeyboardHandles));
    memset(&mKeyboardState, 0, sizeof(mKeyboardState));

    XINPUT_DEBUG_KEYQUEUE_PARAMETERS keyboardSettings;
    keyboardSettings.dwFlags = XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN | XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT | XINPUT_DEBUG_KEYQUEUE_FLAG_KEYUP;
    keyboardSettings.dwQueueSize = 25;
    keyboardSettings.dwRepeatDelay = 500;
    keyboardSettings.dwRepeatInterval = 50;
    XInputDebugInitKeyboardQueue(&keyboardSettings);
}

void input_manager::process_controller()
{
    DWORD insertions = 0;
	DWORD removals = 0;
    if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &insertions, &removals) == TRUE)
	{
		for (int i = 0; i < XGetPortCount(); i++)
		{
			if ((insertions & 1) == 1)
			{
				mControllerHandles[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD, i, XDEVICE_NO_SLOT, NULL);
			}
			if ((removals & 1) == 1)
			{
				XInputClose(mControllerHandles[i]);
				mControllerHandles[i] = NULL;
			}
			insertions = insertions >> 1;
			removals = removals >> 1;
		}
	}

    for (int i = 0; i < XGetPortCount(); i++)
	{
        XINPUT_STATE controllerInputState;
        if (mControllerHandles[i] == NULL || XInputGetState(mControllerHandles[i], &controllerInputState) != 0) 
		{
			continue;
		}

        if (mControllerLastPacketNumber[i] != controllerInputState.dwPacketNumber)
        {
            memcpy(&mControllerStatesPrevious[i], &mControllerStatesCurrent[i], sizeof(ControllerState));
            mControllerStatesCurrent[i].buttons[CONTROLLER_A_BUTTON] = controllerInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] > 32;
            mControllerStatesCurrent[i].buttons[CONTROLLER_B_BUTTON] = controllerInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] > 32;
            mControllerStatesCurrent[i].buttons[CONTROLLER_X_BUTTON] = controllerInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] > 32;
            mControllerStatesCurrent[i].buttons[CONTROLLER_Y_BUTTON] = controllerInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] > 32;
            mControllerStatesCurrent[i].buttons[CONTROLLER_BLACK_BUTTON] = controllerInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > 32;
            mControllerStatesCurrent[i].buttons[CONTROLLER_WHITE_BUTTON] = controllerInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] > 32;
            mControllerStatesCurrent[i].buttons[CONTROLLER_LTRIGGER_BUTTON] = controllerInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 32;
            mControllerStatesCurrent[i].buttons[CONTROLLER_RTRIGGER_BUTTON] = controllerInputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 32;
            mControllerStatesCurrent[i].buttons[CONTROLLER_DPAD_UP_BUTTON] = (controllerInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
            mControllerStatesCurrent[i].buttons[CONTROLLER_DPAD_DOWN_BUTTON] = (controllerInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
            mControllerStatesCurrent[i].buttons[CONTROLLER_DPAD_LEFT_BUTTON] = (controllerInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
            mControllerStatesCurrent[i].buttons[CONTROLLER_DPAD_RIGHT_BUTTON] = (controllerInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
            mControllerStatesCurrent[i].buttons[CONTROLLER_START_BUTTON] = (controllerInputState.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
            mControllerStatesCurrent[i].buttons[CONTROLLER_BACK_BUTTON] = (controllerInputState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
            mControllerStatesCurrent[i].buttons[CONTROLLER_LTHUMB_BUTTON] = (controllerInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
            mControllerStatesCurrent[i].buttons[CONTROLLER_RTHUMB_BUTTON] = (controllerInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
            
            const float sensitivity = 10.0f;
            const float acceleration = 1.6f;
            const float friction = 0.85f;
            const float maxSpeed = 15.0f;
            const float deadzone = 0.25f;

            float lx = controllerInputState.Gamepad.sThumbLX / 32768.0f;
            float ly = -(controllerInputState.Gamepad.sThumbLY / 32768.0f);
            lx = fabsf(lx) < deadzone ? 0 : (lx - math::copy_sign(deadzone, lx)) / (1.0f - deadzone);
            ly = fabsf(ly) < deadzone ? 0 : (ly - math::copy_sign(deadzone, ly)) / (1.0f - deadzone);
            velocity_x = math::clamp_float((velocity_x + lx * acceleration) * friction, -maxSpeed, maxSpeed);
            velocity_y = math::clamp_float((velocity_y + ly * acceleration) * friction, -maxSpeed, maxSpeed);
            mControllerStatesCurrent[i].thumb_left_dx = lx * sensitivity;
            mControllerStatesCurrent[i].thumb_left_dy = ly * sensitivity;

            mMousePosition.x = math::clamp_float(mMousePosition.x + mControllerStatesCurrent[i].thumb_left_dx, 0, (float)graphics::getWidth());
            mMousePosition.y = math::clamp_float(mMousePosition.y + mControllerStatesCurrent[i].thumb_left_dy, 0, (float)graphics::getHeight());
            mControllerLastPacketNumber[i] = controllerInputState.dwPacketNumber;
        }
    }
}

void input_manager::process_mouse()
{
    DWORD insertions = 0;
	DWORD removals = 0;
    if (XGetDeviceChanges(XDEVICE_TYPE_DEBUG_MOUSE, &insertions, &removals) == TRUE)
	{
		for (int i = 0; i < XGetPortCount(); i++)
		{
			if ((insertions & 1) == 1)
			{
				mMouseHandles[i] = XInputOpen(XDEVICE_TYPE_DEBUG_MOUSE, i, XDEVICE_NO_SLOT, NULL);
			}
			if ((removals & 1) == 1)
			{
				XInputClose(mMouseHandles[i]);
				mMouseHandles[i] = NULL;
			}
			insertions = insertions >> 1;
			removals = removals >> 1;
		}
	}

	for (int i = 0; i < XGetPortCount(); i++)
	{
        XINPUT_STATE mouseInputState;
		if (mMouseHandles[i] == NULL || XInputGetState(mMouseHandles[i], &mouseInputState) != 0) 
		{
			continue;
		}

        if (mMouseLastPacketNumber[i] != mouseInputState.dwPacketNumber)
        {
            memcpy(&mMouseStatesPrevious[i], &mouseInputState, sizeof(MouseState));
            mMouseStatesCurrent[i].dx = mouseInputState.DebugMouse.cMickeysX;
            mMouseStatesCurrent[i].dy = mouseInputState.DebugMouse.cMickeysY;
            mMouseStatesCurrent[i].dz = mouseInputState.DebugMouse.cWheel;
            mMouseStatesCurrent[i].buttons[MOUSE_LEFT_BUTTON] = (mouseInputState.DebugMouse.bButtons & XINPUT_DEBUG_MOUSE_LEFT_BUTTON) != 0;
            mMouseStatesCurrent[i].buttons[MOUSE_RIGHT_BUTTON] = (mouseInputState.DebugMouse.bButtons & XINPUT_DEBUG_MOUSE_RIGHT_BUTTON) != 0;
            mMouseStatesCurrent[i].buttons[MOUSE_MIDDLE_BUTTON] = (mouseInputState.DebugMouse.bButtons & XINPUT_DEBUG_MOUSE_MIDDLE_BUTTON) != 0;
            mMouseStatesCurrent[i].buttons[MOUSE_EXTRA_BUTTON1] = (mouseInputState.DebugMouse.bButtons & XINPUT_DEBUG_MOUSE_XBUTTON1) != 0;
            mMouseStatesCurrent[i].buttons[MOUSE_EXTRA_BUTTON2] = (mouseInputState.DebugMouse.bButtons & XINPUT_DEBUG_MOUSE_XBUTTON2) != 0;
            mMousePosition.x = math::clamp_float(mMousePosition.x + mMouseStatesCurrent[i].dx, 0, (float)graphics::getWidth());
            mMousePosition.y = math::clamp_float(mMousePosition.y + mMouseStatesCurrent[i].dy, 0, (float)graphics::getHeight());
            mMouseLastPacketNumber[i] = mouseInputState.dwPacketNumber;
        }
    }
}

void input_manager::process_keyboard()
{
    DWORD insertions = 0;
	DWORD removals = 0;
    if (XGetDeviceChanges(XDEVICE_TYPE_DEBUG_KEYBOARD, &insertions, &removals) == TRUE)
	{
		for (int i = 0; i < XGetPortCount(); i++)
		{
			if ((insertions & 1) == 1)
			{
                XINPUT_POLLING_PARAMETERS pollValues;
                pollValues.fAutoPoll = TRUE;
                pollValues.fInterruptOut = TRUE;
                pollValues.bInputInterval = 32;
                pollValues.bOutputInterval = 32;
                pollValues.ReservedMBZ1 = 0;
                pollValues.ReservedMBZ2 = 0;
				mKeyboardHandles[i] = XInputOpen(XDEVICE_TYPE_DEBUG_KEYBOARD, i, XDEVICE_NO_SLOT, &pollValues);
			}
			if ((removals & 1) == 1)
			{
				XInputClose(mKeyboardHandles[i]);
				mKeyboardHandles[i] = NULL;
			}
			insertions = insertions >> 1;
			removals = removals >> 1;
		}
	}

    
    mKeyboardState.key_down = false; 
    for (int i = 0; i < XGetPortCount(); i++)
	{
        XINPUT_DEBUG_KEYSTROKE currentKeyStroke;
        memset(&currentKeyStroke, 0, sizeof(currentKeyStroke));
		if (mKeyboardHandles[i] == NULL || XInputDebugGetKeystroke(&currentKeyStroke) != 0)
        {
            continue;
        }

        mKeyboardState.key_down = (currentKeyStroke.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_KEYUP) == 0 && currentKeyStroke.Ascii != 0 && currentKeyStroke.VirtualKey != 0;
        mKeyboardState.ascii = currentKeyStroke.Ascii;
        mKeyboardState.virtual_key = currentKeyStroke.VirtualKey;
        mKeyboardState.button[KEYBOARD_CTRL_BUTTON] = (currentKeyStroke.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_CTRL) != 0;          
        mKeyboardState.button[KEYBOARD_SHIFT_BUTTON] = (currentKeyStroke.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_SHIFT) != 0;
        mKeyboardState.button[KEYBOARD_ALT_BUTTON] = (currentKeyStroke.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_ALT) != 0;
        mKeyboardState.button[KEYBOARD_CAPSLOCK_BUTTON] = (currentKeyStroke.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_CAPSLOCK) != 0;
        mKeyboardState.button[KEYBOARD_NUMLOCK_BUTTON] = (currentKeyStroke.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_NUMLOCK) != 0;
        mKeyboardState.button[KEYBOARD_SCROLLLOCK_BUTTON] = (currentKeyStroke.Flags & XINPUT_DEBUG_KEYSTROKE_FLAG_SCROLLLOCK) != 0;
    }
}

bool input_manager::controller_pressed(CONTROLLER_BUTTON button, int port)
{
	for (int i = 0; i < XGetPortCount(); i++)
	{
		if (port >= 0 && port != i || mControllerHandles[i] == NULL)
		{
			continue;
		}
		if (mControllerStatesCurrent[i].buttons[button] == true && mControllerStatesPrevious[i].buttons[button] == false)
		{
			return true;
		}
	}
	return false;
}

bool input_manager::mouse_pressed(MOUSE_BUTTON button, int port)
{
	for (int i = 0; i < XGetPortCount(); i++)
	{
		if (port >= 0 && port != i || mMouseHandles[i] == NULL)
		{
			continue;
		}
		if (mMouseStatesCurrent[i].buttons[button] == true && mMouseStatesPrevious[i].buttons[button] == false)
		{
			return true;
		}
	}
	return false;
}

bool input_manager::try_get_controller_state(int port, ControllerState* controllerState)
{
    if (controllerState != NULL)
    {
	    for (int i = 0; i < XGetPortCount(); i++)
	    {
		    if (port >= 0 && port != i || mControllerHandles[i] == NULL)
		    {
			    continue;
		    }
            *controllerState = mControllerStatesCurrent[i];
            return true;
	    }
    }
	return false;
}

bool input_manager::try_get_mouse_state(int port, MouseState* mouseState)
{
    if (mouseState != NULL)
    {
	    for (int i = 0; i < XGetPortCount(); i++)
	    {
		    if (port >= 0 && port != i || mMouseHandles[i] == NULL)
		    {
			    continue;
		    }
            *mouseState = mMouseStatesCurrent[i];
            return true;
	    }
    }
	return false;
}

bool input_manager::has_controller(int port)
{
	for (int i = 0; i < XGetPortCount(); i++)
	{
		if (port >= 0 && port != i || mControllerHandles[i] == NULL)
		{
			continue;
		}
        return true;
	}
	return false;
}

bool input_manager::has_mouse(int port)
{
	for (int i = 0; i < XGetPortCount(); i++)
	{
		if (port >= 0 && port != i || mMouseHandles[i] == NULL)
		{
			continue;
		}
        return true;
	}
	return false;
}

bool input_manager::try_get_keyboard_state(int port, KeyboardState* keyboardState)
{
    if (keyboardState != NULL)
    {
	    for (int i = 0; i < XGetPortCount(); i++)
	    {
		    if (port >= 0 && port != i || mKeyboardHandles[i] == NULL)
		    {
			    continue;
		    }
            *keyboardState = mKeyboardState;
            return true;
	    }
    }
	return false;
}

void input_manager::pump_input(nk_context *context)
{
    process_controller();
    process_mouse();
    process_keyboard();

    nk_input_begin(context);

    KeyboardState keyboardState;
    memset(&keyboardState, 0, sizeof(keyboardState));
    if (try_get_keyboard_state(-1, &keyboardState))
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

    nk_input_motion(context, (int)mMousePosition.x, (int)mMousePosition.y);

    MouseState mouseState;
    memset(&mouseState, 0, sizeof(mouseState));
    if (try_get_mouse_state(-1, &mouseState))
    {
        nk_input_button(context, NK_BUTTON_LEFT, (int)mMousePosition.x, (int)mMousePosition.y, mouseState.buttons[MOUSE_LEFT_BUTTON]);
        nk_input_button(context, NK_BUTTON_MIDDLE, (int)mMousePosition.x, (int)mMousePosition.y, mouseState.buttons[MOUSE_MIDDLE_BUTTON]);
        nk_input_button(context, NK_BUTTON_RIGHT, (int)mMousePosition.x, (int)mMousePosition.y, mouseState.buttons[MOUSE_RIGHT_BUTTON]);
    }

    ControllerState controllerState;
    memset(&controllerState, 0, sizeof(controllerState));
    if (try_get_controller_state(-1, &controllerState))
    {
        nk_input_button(context, NK_BUTTON_LEFT, (int)mMousePosition.x, (int)mMousePosition.y, controllerState.buttons[CONTROLLER_A_BUTTON]);
        nk_input_button(context, NK_BUTTON_MIDDLE, (int)mMousePosition.x, (int)mMousePosition.y, controllerState.buttons[CONTROLLER_X_BUTTON]);
        nk_input_button(context, NK_BUTTON_RIGHT, (int)mMousePosition.x, (int)mMousePosition.y, controllerState.buttons[CONTROLLER_B_BUTTON]);
    }

    nk_input_end(context);
}

MousePosition input_manager::get_mouse_position()
{
    return mMousePosition;
}
