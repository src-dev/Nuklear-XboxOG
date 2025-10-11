#include "renderer.h"
#include "graphics.h"

#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#include "nuklear.h"

typedef struct nk_vertex 
{
    float position[3];
    nk_byte col[4];
    float uv[2];
} nk_vertex;

nk_buffer _commands;
nk_context _context;
nk_font* _font;

nk_convert_config _config;
D3DTexture* _font_texture;

bool renderer::init()
{
    graphics::createDevice();
    nk_buffer_init_default(&_commands);

    nk_font_atlas atlas;
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);
    _font = nk_font_atlas_add_default(&atlas, 13, 0);

    int image_width;
    int image_height;
    const void* image_data = nk_font_atlas_bake(&atlas, &image_width, &image_height, NK_FONT_ATLAS_RGBA32);

    _font_texture = graphics::createImage((uint8_t*)image_data, D3DFMT_A8R8G8B8, image_width, image_height);

    nk_draw_null_texture null_texture;
    nk_font_atlas_end(&atlas, nk_handle_ptr(_font_texture), &null_texture);
    nk_init_default(&_context, &_font->handle);

    static const nk_draw_vertex_layout_element vertex_layout[] = {
        {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(nk_vertex, position)},
        {NK_VERTEX_COLOR, NK_FORMAT_B8G8R8A8, NK_OFFSETOF(nk_vertex, col)},
        {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(nk_vertex, uv)},
        {NK_VERTEX_LAYOUT_END}
    };
    NK_MEMSET(&_config, 0, sizeof(_config));
    _config.vertex_layout = vertex_layout;
    _config.vertex_size = sizeof(struct nk_vertex);
    _config.vertex_alignment = NK_ALIGNOF(struct nk_vertex);
    _config.tex_null = null_texture;
    _config.circle_segment_count = 22;
    _config.curve_segment_count = 22;
    _config.arc_segment_count = 22;
    _config.global_alpha = 1.0f;
    _config.shape_AA = NK_ANTI_ALIASING_ON;
    _config.line_AA = NK_ANTI_ALIASING_ON;

    return true;
}

void renderer::render(uint32_t background_color)
{
    graphics::getDevice()->BeginScene();
    graphics::getDevice()->Clear(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, background_color, 1.0f, 0L);

    /* convert shapes into vertexes */
    nk_buffer vertex_buffer;
    nk_buffer_init_default(&vertex_buffer);
    nk_buffer index_buffer;
    nk_buffer_init_default(&index_buffer);
    nk_convert(&_context, &_commands, &vertex_buffer, &index_buffer, &_config);
    const nk_draw_index* offset = (const nk_draw_index*)nk_buffer_memory_const(&index_buffer);
    UINT vertex_count = (UINT)vertex_buffer.needed / sizeof(nk_vertex);

    /* iterate over and execute each draw command */
    const nk_draw_command *command;
    nk_draw_foreach(command, &_context, &_commands)
    {
        if (!command->elem_count) 
        {
            continue;
        }

        graphics::getDevice()->SetTexture(0, (D3DTexture*)command->texture.ptr);
    
        const struct nk_rect null_rect = nk_get_null_rect();
        bool isNullRect = memcmp(&null_rect, &command->clip_rect, sizeof(struct nk_rect)) == 0;
        if (isNullRect)
        {
            D3DVIEWPORT8 vp = { 0, 0, graphics::getWidth(), graphics::getHeight(), 0.0f, 1.0f };
            graphics::getDevice()->SetViewport(&vp);
        }
        else
        {
            D3DVIEWPORT8 vp = { (DWORD)command->clip_rect.x, (DWORD)command->clip_rect.y, (DWORD)command->clip_rect.w, (DWORD)command->clip_rect.h, 0.0f, 1.0f };
            graphics::getDevice()->SetViewport(&vp);
        }

        graphics::getDevice()->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, vertex_count, command->elem_count/3, offset, D3DFMT_INDEX16, nk_buffer_memory_const(&vertex_buffer), sizeof(nk_vertex));
        offset += command->elem_count;
    }

    nk_buffer_free(&vertex_buffer);
    nk_buffer_free(&index_buffer);

    nk_clear(&_context);
    nk_buffer_clear(&_commands);

	graphics::getDevice()->EndScene();
	graphics::getDevice()->Present(NULL, NULL, NULL, NULL);
}

nk_context* renderer::get_context()
{
    return &_context;
}

nk_font* renderer::get_font()
{
    return _font;
}

void renderer::mouse_pointer(int x, int y)
{
    if (nk_begin(&_context, "#overlay", nk_rect(0, 0, graphics::getWidth(), graphics::getHeight()), NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR))
    {
        _context.style.window.fixed_background = nk_style_item_color(nk_rgba(0,0,0,0));

        float points[14];
        points[0] = (float)x;      
        points[1] = (float)y;
        points[2] = x + 6.0f;  
        points[3] = y;
        points[4] = x + 4.0f;  
        points[5] = y + 2;
        points[6] = x + 7.0f;  
        points[7] = y + 5.0f;
        points[8] = x + 5.0f;  
        points[9] = y + 7.0f;
        points[10] = x + 2.0f;  
        points[11] = y + 4.0f;
        points[12] = x;  
        points[13] = y + 6.0f;
        nk_command_buffer* command_buffer = nk_window_get_canvas(&_context);
        nk_fill_polygon(command_buffer, points, 5, nk_rgb(255, 255, 255));
        nk_stroke_polygon(command_buffer, points, 7, 1.0f, nk_rgb(0, 0, 0)); 
    }
    nk_end(&_context);
}