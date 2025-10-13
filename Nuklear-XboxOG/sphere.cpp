#include "sphere.h"
#include "graphics.h"
#include "renderer.h"
#include "sphere_image.h"

float _angle;
static D3DTexture* _sphere_texture;
static nk_vertex* _sphere_mesh;

#define LAT_DIV 30  
#define LON_DIV 30
#define TOTAL_VERTICES ((LAT_DIV + 1) * (LON_DIV + 1))
#define TOTAL_TRIANGLES (LAT_DIV * LON_DIV * 2)

static nk_vertex* create_sphere()
{
    float radius = 10.0f;
    int vertexIndex = 0;
    int triangleIndex = 0;

    const float PI = 3.14159265358979323846f;

	nk_vertex* tempVertices = (nk_vertex*)malloc(TOTAL_TRIANGLES * 3 * sizeof(nk_vertex));

    nk_vertex vertices[TOTAL_VERTICES];
    for (int i = 0; i <= LAT_DIV; ++i) 
    {
        float theta = PI * i / LAT_DIV;
        float v = (float)i / LAT_DIV;    

        for (int j = 0; j <= LON_DIV; ++j) 
        {
            float phi = 2.0f * PI * j / LON_DIV; 
            float u = (float)j / LON_DIV;    

            float x = radius * sin(theta) * cos(phi);
            float y = radius * cos(theta);
            float z = radius * sin(theta) * sin(phi);

            vertices[vertexIndex].position[0] = x;
            vertices[vertexIndex].position[1] = y;
            vertices[vertexIndex].position[2] = z;
            vertices[vertexIndex].col[0] = 0xff;
            vertices[vertexIndex].col[1] = 0x7f;
            vertices[vertexIndex].col[2] = 0x5a;
            vertices[vertexIndex].col[3] = 0xff;
            vertices[vertexIndex].uv[0] = u;
            vertices[vertexIndex].uv[1] = v;
            vertexIndex++;
        }
    }

    char* data_offset = (char*)tempVertices;
    for (int i = 0; i < LAT_DIV; ++i) 
    {
        for (int j = 0; j < LON_DIV; ++j) 
        {
            int v1 = i * (LON_DIV + 1) + j;
            int v2 = v1 + 1;
            int v3 = (i + 1) * (LON_DIV + 1) + j;
            int v4 = v3 + 1;

            vertices[v4].uv[0] = 1.0f;
            vertices[v4].uv[1] = 1.0f;
            vertices[v2].uv[0] = 1.0f;
            vertices[v2].uv[1] = 0.0f;
            vertices[v1].uv[0] = 0.0f;
            vertices[v1].uv[1] = 0.0f;

            memcpy(data_offset, &vertices[v1], sizeof(nk_vertex));
            data_offset += sizeof(nk_vertex);
            memcpy(data_offset, &vertices[v2], sizeof(nk_vertex));
            data_offset += sizeof(nk_vertex);
            memcpy(data_offset, &vertices[v4], sizeof(nk_vertex));
            data_offset += sizeof(nk_vertex);

            vertices[v3].uv[0] = 0.0f;
            vertices[v3].uv[1] = 1.0f;
            vertices[v4].uv[0] = 1.0f;
            vertices[v4].uv[1] = 1.0f;
            vertices[v1].uv[0] = 0.0f;
            vertices[v1].uv[1] = 0.0f;

            memcpy(data_offset, &vertices[v1], sizeof(nk_vertex));
            data_offset += sizeof(nk_vertex);
            memcpy(data_offset, &vertices[v4], sizeof(nk_vertex));
            data_offset += sizeof(nk_vertex);
            memcpy(data_offset, &vertices[v3], sizeof(nk_vertex));
            data_offset += sizeof(nk_vertex);
        }
    }

   return tempVertices;
}

void sphere::init()
{
    _angle = 0;
    _sphere_texture = graphics::createImage((uint8_t*)sphere_image, D3DFMT_A8R8G8B8, 256, 256);
    _sphere_mesh = create_sphere();
}

void sphere::render()
{
    const float deg_to_rad = 0.01745329252f;

    _angle += 0.05f;
    if (_angle > 360.0f)
    {
        _angle -= 360.f;
    }

    D3DXMATRIX matWorld;
    D3DXMatrixRotationY(&matWorld, -_angle * deg_to_rad);

    D3DXVECTOR3 cameraPosition(0.0f, 0.1f, 9.9f);
    D3DXVECTOR3 target(0.0f, 0.0f, 7.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);   

    D3DXMATRIX matView;
    D3DXMatrixLookAtLH(&matView, &cameraPosition, &target, &up);

    D3DXMATRIX matProjection;
    D3DXMatrixIdentity(&matProjection);
    D3DXMatrixPerspectiveFovLH(&matProjection, 45 * deg_to_rad, (float)graphics::getWidth() / (float)graphics::getHeight(), 0.1f, 100.0f);
   
    graphics::getDevice()->SetTransform( D3DTS_WORLD, &matWorld);
    graphics::getDevice()->SetTransform(D3DTS_VIEW, &matView);
    graphics::getDevice()->SetTransform(D3DTS_PROJECTION, &matProjection);

    graphics::getDevice()->SetRenderState(D3DRS_FOGENABLE, TRUE);
    graphics::getDevice()->SetRenderState(D3DRS_FOGCOLOR, 0xff000000); 
    graphics::getDevice()->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
    float fogStart = 7.0f;
    float fogEnd = 20.0f;
    graphics::getDevice()->SetRenderState(D3DRS_FOGSTART, *(DWORD*)&fogStart);
    graphics::getDevice()->SetRenderState(D3DRS_FOGEND, *(DWORD*)&fogEnd);

    graphics::getDevice()->SetTexture(0, _sphere_texture);
    graphics::getDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, TOTAL_TRIANGLES, _sphere_mesh, sizeof(nk_vertex));

    graphics::getDevice()->SetRenderState(D3DRS_FOGENABLE, FALSE);
}