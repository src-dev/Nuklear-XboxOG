#include "stdafx.h"
#include "graphics.h"
#include "renderer.h"
#include "input_manager.h"
#include "Demos\controls.h"
#include "Demos\canvas.h"

void __cdecl main()
{
    renderer::init();
    input_manager::init();

    nk_context* context = renderer::get_context();

    while (true)
    {
        input_manager::pump_input(context);

        canvas::render();
        controls::render();

        MouseState mouseState;
        memset(&mouseState, 0, sizeof(mouseState));
        if (input_manager::try_get_mouse_state(-1, &mouseState))
        {
            renderer::mouse_pointer(mouseState.x, mouseState.y);
        }

        renderer::render(0xff333333);
    }
}