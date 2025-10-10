#include "input_manager.h"
#include "graphics.h"
#include "math.h"

namespace
{
	bool mInitialized = false;
	HANDLE mMouseHandles[XGetPortCount()];
    XINPUT_STATE mMouseInputStates[XGetPortCount()];
    DWORD mMouseLastPacketNumber[XGetPortCount()];
    MouseState mMouseStatesCurrent[XGetPortCount()];
    MouseState mMouseStatesPrevious[XGetPortCount()];
}

void input_manager::process()
{
	if (mInitialized == false)
	{
		mInitialized = true;
		XInitDevices(0, 0);
        memset(mMouseHandles, 0, sizeof(mMouseHandles));
        memset(mMouseInputStates, 0, sizeof(mMouseInputStates));
        memset(mMouseLastPacketNumber, 0, sizeof(mMouseLastPacketNumber));
        memset(mMouseStatesCurrent, 0, sizeof(mMouseStatesCurrent));
        memset(mMouseStatesPrevious, 0, sizeof(mMouseStatesPrevious));
	}

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
		if (mMouseHandles[i] == NULL)
		{
			continue;
		}

		if (XInputGetState(mMouseHandles[i], &mMouseInputStates[i]) != 0) 
		{
			continue;
		}

        if (mMouseLastPacketNumber[i] != mMouseInputStates[i].dwPacketNumber)
        {
            memcpy(&mMouseStatesPrevious[i], &mMouseStatesCurrent[i], sizeof(MouseState));
            mMouseStatesCurrent[i].dx = mMouseInputStates[i].DebugMouse.cMickeysX;
            mMouseStatesCurrent[i].dy = mMouseInputStates[i].DebugMouse.cMickeysY;
            mMouseStatesCurrent[i].x = math::clamp_int(mMouseStatesPrevious[i].x + mMouseStatesCurrent[i].dx, 0, graphics::getWidth());
            mMouseStatesCurrent[i].y = math::clamp_int(mMouseStatesPrevious[i].y + mMouseStatesCurrent[i].dy, 0, graphics::getHeight());
            mMouseStatesCurrent[i].dz = mMouseInputStates[i].DebugMouse.cWheel;
            mMouseStatesCurrent[i].button[MOUSE_LEFT_BUTTON] = (mMouseInputStates[i].DebugMouse.bButtons & XINPUT_DEBUG_MOUSE_LEFT_BUTTON) != 0;
            mMouseStatesCurrent[i].button[MOUSE_RIGHT_BUTTON] = (mMouseInputStates[i].DebugMouse.bButtons & XINPUT_DEBUG_MOUSE_RIGHT_BUTTON) != 0;
            mMouseStatesCurrent[i].button[MOUSE_MIDDLE_BUTTON] = (mMouseInputStates[i].DebugMouse.bButtons & XINPUT_DEBUG_MOUSE_MIDDLE_BUTTON) != 0;
            mMouseStatesCurrent[i].button[MOUSE_EXTRA_BUTTON1] = (mMouseInputStates[i].DebugMouse.bButtons & XINPUT_DEBUG_MOUSE_XBUTTON1) != 0;
            mMouseStatesCurrent[i].button[MOUSE_EXTRA_BUTTON2] = (mMouseInputStates[i].DebugMouse.bButtons & XINPUT_DEBUG_MOUSE_XBUTTON2) != 0;
            mMouseLastPacketNumber[i] = mMouseInputStates[i].dwPacketNumber;
        }
    }
}

bool input_manager::mouse_pressed(MOUSE_BUTTON button, int port)
{
	for (int i = 0; i < XGetPortCount(); i++)
	{
		if (port >= 0 && port != i || mMouseHandles[i] == NULL)
		{
			continue;
		}
		if (mMouseStatesCurrent[i].button[button] == true && mMouseStatesPrevious[i].button[button] == false)
		{
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
