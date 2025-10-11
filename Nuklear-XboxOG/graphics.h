#pragma once

#include <xtl.h>
#include "stdint.h"

typedef struct {
    DWORD dwWidth;
    DWORD dwHeight;
    BOOL  fProgressive;
    BOOL  fWideScreen;
	DWORD dwFreq;
} DISPLAY_MODE;

class graphics
{
public:
    static bool supportsMode(DISPLAY_MODE mode, DWORD dwVideoStandard, DWORD dwVideoFlags);
    static bool createDevice();
    static void begin_stencil(float x, float y, float w, float h);
    static void end_stencil();
    static LPDIRECT3DDEVICE8 getDevice();
    static void swizzle(const void *src, const uint32_t& depth, const uint32_t& width, const uint32_t& height, void *dest);
    static D3DTexture* createImage(uint8_t* imageData, D3DFORMAT format, int width, int height);
    static int getWidth();
    static int getHeight();
};
