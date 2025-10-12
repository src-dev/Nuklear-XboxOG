#include "stdafx.h"
#include "graphics.h"
#include "renderer.h"
#include "input_manager.h"
#include "Demos\controls.h"
#include "Demos\canvas.h"
#include "Demos\calculator.h"
#include "Demos\overview.h"

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
        calculator::render();
        overview::render();

        renderer::render(0xff333333);

        if (input_manager::has_mouse(-1))
        {
            renderer::mouse_pointer();
        }

        renderer::present();
    }
}