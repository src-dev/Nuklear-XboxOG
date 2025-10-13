#include "renderer.h"
#include "graphics.h"
#include "input_manager.h"
#include "debug.h"
#include "futura_thin_ttf.h"

#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#include "nuklear.h"

#define MAX_INDICES_PER_BATCH 16383

nk_buffer _commands;
nk_context _context;
nk_font* _font;

nk_convert_config _config;
D3DTexture* _font_texture;
D3DTexture* _mouse_texture;

bool renderer::init()
{
    graphics::createDevice();
    nk_buffer_init_default(&_commands);

    nk_font_atlas atlas;
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);

    //_font = nk_font_atlas_add_default(&atlas, 13, 0);
    _font = nk_font_atlas_add_from_memory(&atlas, (void*)future_thin_ttf, sizeof(future_thin_ttf), 12, 0);

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

    uint8_t mouse_pointer[256] = {
        000,000,000,255, 000,000,000,255, 000,000,000,255, 000,000,000,255, 000,000,000,255, 000,000,000,255, 000,000,000,255, 000,000,000,000,
        000,000,000,255, 255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255, 000,000,000,255, 000,000,000,000, 000,000,000,000,
        000,000,000,255, 255,255,255,255, 255,255,255,255, 255,255,255,255, 000,000,255,000, 000,000,000,000, 000,000,000,000, 000,000,000,000,
        000,000,000,255, 255,255,255,255, 255,255,255,255, 255,255,000,255, 255,255,255,255, 000,000,000,255, 000,000,000,000, 000,000,000,000,
        000,000,000,255, 255,255,255,255, 000,000,000,255, 255,255,255,255, 255,255,255,255, 255,255,255,255, 000,000,000,255, 000,000,000,000,
        000,000,000,255, 000,000,000,255, 000,000,000,000, 000,000,000,255, 255,255,255,255, 255,255,255,255, 255,255,255,255, 000,000,000,255,
        000,000,000,255, 000,000,000,000, 000,000,000,000, 000,000,000,000, 000,000,000,255, 255,255,255,255, 000,000,000,255, 000,000,000,000,
        000,000,000,000, 000,000,000,000, 000,000,000,000, 000,000,000,000, 000,000,000,000, 000,000,000,255, 000,000,000,000, 000,000,000,000
    };

    _mouse_texture = graphics::createImage(mouse_pointer, D3DFMT_A8R8G8B8, 8, 8);

    return true;
}

void renderer::begin_render(uint32_t background_color)
{
    graphics::getDevice()->BeginScene();
    graphics::getDevice()->Clear(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, background_color, 1.0f, 0L);
}

void renderer::render()
{
    const float L = 0.5f;
    const float R = (float)graphics::getWidth() + 0.5f;
    const float T = 0.5f;
    const float B = (float)graphics::getHeight() + 0.5f;
    float matrix[4][4] = {
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f },
    };
    matrix[0][0] = 2.0f / (R - L);
    matrix[1][1] = 2.0f / (T - B);
    matrix[3][0] = (R + L) / (L - R);
    matrix[3][1] = (T + B) / (B - T);
    
    D3DXMATRIX matProjection;
    memcpy(matProjection, matrix, sizeof(matrix));

    graphics::getDevice()->SetTransform(D3DTS_PROJECTION, &matProjection);

	D3DXMATRIX  matView;
    D3DXMatrixIdentity(&matView);
    graphics::getDevice()->SetTransform( D3DTS_VIEW, &matView);

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	graphics::getDevice()->SetTransform( D3DTS_WORLD, &matWorld);

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

        if (!isNullRect)
        {
            graphics::begin_stencil(command->clip_rect.x, command->clip_rect.y, command->clip_rect.w, command->clip_rect.h);
        }

        //graphics::getDevice()->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, vertex_count, command->elem_count/3, offset, D3DFMT_INDEX16, nk_buffer_memory_const(&vertex_buffer), sizeof(nk_vertex));

        int remaining = command->elem_count;
        int indexOffset = 0;
        while (remaining > 0) 
        {
            int batchCount = min(MAX_INDICES_PER_BATCH, remaining);
            graphics::getDevice()->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, vertex_count, batchCount / 3, offset + indexOffset, D3DFMT_INDEX16, nk_buffer_memory_const(&vertex_buffer), sizeof(nk_vertex));
            remaining -= batchCount;
            indexOffset += batchCount;
        }

        if (!isNullRect)
        {
            graphics::end_stencil();
        }

        offset += command->elem_count;
    }

    nk_buffer_free(&vertex_buffer);
    nk_buffer_free(&index_buffer);

    nk_clear(&_context);
    nk_buffer_clear(&_commands);
}

void renderer::end_render()
{
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

void renderer::mouse_pointer()
{
    if (!input_manager::has_mouse(-1) && !input_manager::has_controller(-1))
    {
        return;
    }
    struct nk_vec2 pos = _context.input.mouse.pos;
    nk_vertex quad[4] =
    {
        { pos.x, pos.y, 0.0f, 0xff, 0xff, 0xff, 0xff, 0.0f, 0.0f },
        { pos.x + 8, pos.y, 0.0f, 0xff, 0xff, 0xff, 0xff, 1.0f, 0.0f },
        { pos.x, pos.y + 8, 0.0f, 0xff, 0xff, 0xff, 0xff, 0.0f, 1.0f },
        { pos.x + 8, pos.y + 8, 0.0f, 0xff, 0xff, 0xff, 0xff, 1.0f, 1.0f }
    };
    graphics::getDevice()->SetTexture(0, _mouse_texture);
    graphics::getDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(nk_vertex));
}