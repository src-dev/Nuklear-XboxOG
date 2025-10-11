#include "StdAfx.h"
#include ".\graphics.h"
#include "stdint.h"

static int mWidth;
static int mHeight;
static LPDIRECT3DDEVICE8 mD3dDevice;

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

DISPLAY_MODE displayModes[] =
{
    {   720,    480,    TRUE,   TRUE,  60 },         // 720x480 progressive 16x9
    {   720,    480,    TRUE,   FALSE, 60 },         // 720x480 progressive 4x3
    {   720,    480,    FALSE,  TRUE,  50 },         // 720x480 interlaced 16x9 50Hz
    {   720,    480,    FALSE,  FALSE, 50 },         // 720x480 interlaced 4x3  50Hz
    {   720,    480,    FALSE,  TRUE,  60 },         // 720x480 interlaced 16x9
    {   720,    480,    FALSE,  FALSE, 60 },         // 720x480 interlaced 4x3

	// Width  Height Progressive Widescreen

	// HDTV Progressive Modes
    {  1280,    720,    TRUE,   TRUE,  60 },         // 1280x720 progressive 16x9

	// EDTV Progressive Modes
    {   720,    480,    TRUE,   TRUE,  60 },         // 720x480 progressive 16x9
    {   640,    480,    TRUE,   TRUE,  60 },         // 640x480 progressive 16x9
    {   720,    480,    TRUE,   FALSE, 60 },         // 720x480 progressive 4x3
    {   640,    480,    TRUE,   FALSE, 60 },         // 640x480 progressive 4x3

	// HDTV Interlaced Modes
	{  1920,   1080,    FALSE,  TRUE,  60 },         // 1920x1080 interlaced 16x9

	// SDTV PAL-50 Interlaced Modes
    {   720,    480,    FALSE,  TRUE,  50 },         // 720x480 interlaced 16x9 50Hz
    {   640,    480,    FALSE,  TRUE,  50 },         // 640x480 interlaced 16x9 50Hz
    {   720,    480,    FALSE,  FALSE, 50 },         // 720x480 interlaced 4x3  50Hz
    {   640,    480,    FALSE,  FALSE, 50 },         // 640x480 interlaced 4x3  50Hz

	// SDTV NTSC / PAL-60 Interlaced Modes
    {   720,    480,    FALSE,  TRUE,  60 },         // 720x480 interlaced 16x9
    {   640,    480,    FALSE,  TRUE,  60 },         // 640x480 interlaced 16x9
    {   720,    480,    FALSE,  FALSE, 60 },         // 720x480 interlaced 4x3
    {   640,    480,    FALSE,  FALSE, 60 },         // 640x480 interlaced 4x3
};

#define NUM_MODES (sizeof(displayModes) / sizeof(displayModes[0]))

bool graphics::supportsMode(DISPLAY_MODE mode, DWORD dwVideoStandard, DWORD dwVideoFlags)
{
    if (mode.dwFreq == 60 && !(dwVideoFlags & XC_VIDEO_FLAGS_PAL_60Hz) && (dwVideoStandard == XC_VIDEO_STANDARD_PAL_I))
	{
		return false;
	}    
    if (mode.dwFreq == 50 && (dwVideoStandard != XC_VIDEO_STANDARD_PAL_I))
	{
		return false;
	}
    if (mode.dwHeight == 480 && mode.fWideScreen && !(dwVideoFlags & XC_VIDEO_FLAGS_WIDESCREEN ))
	{
		return false;
	}
    if (mode.dwHeight == 480 && mode.fProgressive && !(dwVideoFlags & XC_VIDEO_FLAGS_HDTV_480p))
	{
		return false;
	}
    if (mode.dwHeight == 720 && !(dwVideoFlags & XC_VIDEO_FLAGS_HDTV_720p))
	{
		return false;
	}
    if (mode.dwHeight == 1080 && !(dwVideoFlags & XC_VIDEO_FLAGS_HDTV_1080i))
	{
		return false;
	}
    return true;
}

bool graphics::createDevice()
{
	uint32_t videoFlags = XGetVideoFlags();
	uint32_t videoStandard = XGetVideoStandard();
	uint32_t currentMode;
    for (currentMode = 0; currentMode < NUM_MODES-1; currentMode++)
    {
		if (supportsMode(displayModes[currentMode], videoStandard, videoFlags)) 
		{
			break;
		}
    } 

	LPDIRECT3D8 d3d = Direct3DCreate8(D3D_SDK_VERSION);
    if(d3d == NULL)
	{
        return false;
	}

	D3DPRESENT_PARAMETERS params; 
    ZeroMemory(&params, sizeof(params));
	params.BackBufferWidth = displayModes[currentMode].dwWidth;
    params.BackBufferHeight = displayModes[currentMode].dwHeight;
	params.Flags = displayModes[currentMode].fProgressive ? D3DPRESENTFLAG_PROGRESSIVE : D3DPRESENTFLAG_INTERLACED;
    params.Flags |= displayModes[currentMode].fWideScreen ? D3DPRESENTFLAG_WIDESCREEN : 0;
    params.FullScreen_RefreshRateInHz = displayModes[currentMode].dwFreq;
	params.BackBufferFormat = D3DFMT_X8R8G8B8;
    params.BackBufferCount = 1;
    params.EnableAutoDepthStencil = TRUE;
    params.AutoDepthStencilFormat = D3DFMT_D24S8;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    params.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    mWidth = displayModes[currentMode].dwWidth;
    mHeight = displayModes[currentMode].dwHeight;

	LPDIRECT3DDEVICE8 d3dDevice;
    if (FAILED(d3d->CreateDevice(0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, &d3dDevice)))
	{
        return false;
	}
    mD3dDevice = d3dDevice;

    const float L = 0.5f;
    const float R = (float)mWidth + 0.5f;
    const float T = 0.5f;
    const float B = (float)mHeight + 0.5f;
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

	//D3DXMatrixOrthoOffCenterRH(&matrix, 0, (float)mWidth, 0, (float)mHeight, 1.0f, 100.0f);
    mD3dDevice->SetTransform(D3DTS_PROJECTION, &matProjection);

	D3DXMATRIX  matView;
    D3DXMatrixIdentity(&matView);
    mD3dDevice->SetTransform( D3DTS_VIEW, &matView);

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	mD3dDevice->SetTransform( D3DTS_WORLD, &matWorld);

    mD3dDevice->SetVertexShader(D3DFVF_XYZ + D3DFVF_DIFFUSE + D3DFVF_TEX1);

    /* blend state */
    mD3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    mD3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    mD3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    mD3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    /* render state */
    mD3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    mD3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    mD3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    mD3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    /* sampler state */
    mD3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP);
    mD3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP);
    mD3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	mD3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	
    /* texture stage state */
    mD3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    mD3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    mD3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    mD3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    mD3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    mD3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	return true;
}

void graphics::begin_stencil(float x, float y, float w, float h)
{
    mD3dDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
    mD3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
    mD3dDevice->SetRenderState(D3DRS_STENCILREF, 1);
    mD3dDevice->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
    mD3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
    mD3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
    mD3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

    struct Vertex { float x,y,z,rhw; };
    Vertex quad[4] = {
        { (float)x, (float)y, 0.0f, 1.0f },
        { (float)(x+w), (float)y, 0.0f, 1.0f },
        { (float)x, (float)(y+h), 0.0f, 1.0f },
        { (float)(x+w), (float)(y+h), 0.0f, 1.0f },
    };

    mD3dDevice->SetVertexShader(D3DFVF_XYZRHW);
    mD3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(Vertex));
    mD3dDevice->SetVertexShader(D3DFVF_XYZ + D3DFVF_DIFFUSE + D3DFVF_TEX1);

    mD3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
    mD3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
    mD3dDevice->SetRenderState(D3DRS_STENCILREF, 1);
    mD3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
}

void graphics::end_stencil()
{
    mD3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
}

LPDIRECT3DDEVICE8 graphics::getDevice()
{
    return mD3dDevice;
}

void graphics::swizzle(const void *src, const uint32_t& depth, const uint32_t& width, const uint32_t& height, void *dest)
{
  for (UINT y = 0; y < height; y++)
  {
    UINT sy = 0;
    if (y < width)
    {
      for (int bit = 0; bit < 16; bit++)
        sy |= ((y >> bit) & 1) << (2*bit);
      sy <<= 1; // y counts twice
    }
    else
    {
      UINT y_mask = y % width;
      for (int bit = 0; bit < 16; bit++)
        sy |= ((y_mask >> bit) & 1) << (2*bit);
      sy <<= 1; // y counts twice
      sy += (y / width) * width * width;
    }
    BYTE *s = (BYTE *)src + y * width * depth;
    for (UINT x = 0; x < width; x++)
    {
      UINT sx = 0;
      if (x < height * 2)
      {
        for (int bit = 0; bit < 16; bit++)
          sx |= ((x >> bit) & 1) << (2*bit);
      }
      else
      {
        int x_mask = x % (2*height);
        for (int bit = 0; bit < 16; bit++)
          sx |= ((x_mask >> bit) & 1) << (2*bit);
        sx += (x / (2 * height)) * 2 * height * height;
      }
      BYTE *d = (BYTE *)dest + (sx + sy)*depth;
      for (unsigned int i = 0; i < depth; ++i)
        *d++ = *s++;
    }
  }
}

D3DTexture* graphics::createImage(uint8_t* imageData, D3DFORMAT format, int width, int height)
{
	D3DTexture *texture;
	if (FAILED(D3DXCreateTexture(mD3dDevice, width, height, 1, 0, format, D3DPOOL_DEFAULT, &texture)))
	{
		return false;
	}

	D3DSURFACE_DESC surfaceDesc;
	texture->GetLevelDesc(0, &surfaceDesc);

	D3DLOCKED_RECT lockedRect;
	if (SUCCEEDED(texture->LockRect(0, &lockedRect, NULL, 0)))
	{
		uint8_t* tempBuffer = (uint8_t*)malloc(surfaceDesc.Size);
		memset(tempBuffer, 0, surfaceDesc.Size);
		uint8_t* src = imageData;
		uint8_t* dst = tempBuffer;
		for (int32_t y = 0; y < height; y++)
		{
			memcpy(dst, src, width * 4);
			src += width * 4;
			dst += surfaceDesc.Width * 4;
		}
		swizzle(tempBuffer, 4, surfaceDesc.Width, surfaceDesc.Height, lockedRect.pBits);
		free(tempBuffer);
		texture->UnlockRect(0);
	}

	return texture;
}

int graphics::getWidth()
{
    return mWidth;
}
int graphics::getHeight()
{
    return mHeight;
}