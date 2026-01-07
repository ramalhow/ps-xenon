#include "Graphics.hpp"
#include "Common\Log.hpp"
//#include "Shaders.hpp"
#include <assert.h>

GraphicsContext::GraphicsContext() : d3d(nullptr), device(nullptr) {
  ZeroMemory(&presentParameters, sizeof(D3DPRESENT_PARAMETERS));
};

#define MAX_BUFFERS 2
static IDirect3DTexture9 *FRAME_BUFFERS[MAX_BUFFERS];
static int currentBufferIdx = 0;

namespace Graphics {

GraphicsContext Init(Settings &settings) {
  GraphicsContext gc;

  gc.d3d = Direct3DCreate9(D3D_SDK_VERSION);

  // device config
  gc.presentParameters.BackBufferWidth = 1280;
  gc.presentParameters.BackBufferHeight = 720;
  gc.presentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
  gc.presentParameters.FrontBufferFormat = D3DFMT_LE_X8R8G8B8;
  gc.presentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
  gc.presentParameters.Windowed = false;
  gc.presentParameters.EnableAutoDepthStencil = TRUE;
  gc.presentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
  gc.presentParameters.PresentationInterval =
      settings.enableVsync ? D3DPRESENT_INTERVAL_IMMEDIATE
                           : D3DPRESENT_INTERVAL_ONE;
  gc.presentParameters.DisableAutoBackBuffer = TRUE;
  gc.presentParameters.DisableAutoFrontBuffer = TRUE;

  // TODO: learn more about this optional parameters
  //  gc.presentParameters.RingBufferParameters;
  //  gc.presentParameters.VideoScalerParameters;

  // TODO: using the correct BehaviorFlags, we can do threaded rendering using
  // command buffers. but for now, doing all on a single thread is sufficient.
  HRESULT hr = gc.d3d->CreateDevice(0, D3DDEVTYPE_HAL, NULL, 0,
                                    &gc.presentParameters, &gc.device);

  assert(SUCCEEDED(hr));
  Log::GenericLog("Created d3d device");

  for (int i = 0; i < MAX_BUFFERS; i++) {
    hr = gc.device->CreateTexture(gc.presentParameters.BackBufferWidth,
                                  gc.presentParameters.BackBufferHeight, 1, 0,
                                  gc.presentParameters.FrontBufferFormat, 0,
                                  &FRAME_BUFFERS[i], NULL);
    assert(SUCCEEDED(hr));
  }

  Log::GenericLog("Created frontbuffers");

  return gc;
};

void LoadShaders(GraphicsContext &gc) {}

// TODO: understand this draw function and find better ways
// to send constant data directly to the xbox 360 gpu
void Draw(GraphicsContext &gc) {

	/*

  if (in_modelViewProj != NULL)
    pVsShaderTable->SetMatrix(gc.device, in_modelViewProj,
                              (D3DXMATRIX *)&WorldView);

  if (input_vs.texture_size != NULL)
    pVsShaderTable->SetFloatArray(gc.device, input_vs.texture_size,
                                  texture_size, 2);

  if (input_vs.output_size != NULL)
    pVsShaderTable->SetFloatArray(gc.device, input_vs.output_size, output_size,
                                  2);
  if (input_vs.video_size != NULL)
    pVsShaderTable->SetFloatArray(gc.device, input_vs.video_size, video_size,
                                  2);

  if (input_ps.texture_size != NULL)
    pPsShaderTable->SetFloatArray(gc.device, input_ps.texture_size,
                                  texture_size, 2);

  if (input_ps.output_size != NULL)
    pPsShaderTable->SetFloatArray(gc.device, input_ps.output_size, output_size,
                                  2);
  if (input_ps.video_size != NULL)
    pPsShaderTable->SetFloatArray(gc.device, input_ps.video_size, video_size,
                                  2);

  // Set shaders.
  gc.device->SetVertexShader(pVertexShader);
  gc.device->SetPixelShader(pPixelShader);

  // Set texture
  gc.device->SetTexture(0, g_PsxTexture);

  // set some stuff overwritten by xui ...
  if (linearfilter) {
    ///////////////////////////////////////////////////////////////////////////////////
    gc.device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    gc.device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    gc.device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    ///////////////////////////////////////////////////////////////////////////////////
  } else {
    ///////////////////////////////////////////////////////////////////////////////////
    gc.device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    gc.device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    gc.device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    ///////////////////////////////////////////////////////////////////////////////////
  }
  gc.device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  gc.device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
  gc.device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
  gc.device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
  gc.device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
  gc.device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
  gc.device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL);

  // Set the vertex declaration.
  gc.device->SetVertexDeclaration(pVertexDecl);

  // Draw Pcsx surface
  gc.device->DrawPrimitiveUP(D3DPT_RECTLIST, 1, Vertices, sizeof(COLORVERTEX));
  */
};

void SwapBuffers(GraphicsContext &gc) {

  IDirect3DTexture9 *currentBuffer = FRAME_BUFFERS[currentBufferIdx];

  gc.device->SynchronizeToPresentationInterval();

  gc.device->Resolve(D3DRESOLVE_RENDERTARGET0 | D3DRESOLVE_CLEARRENDERTARGET |
                         D3DRESOLVE_CLEARDEPTHSTENCIL,
                     NULL, currentBuffer, NULL, 0, 0, NULL, 0.0f, 0, NULL);

  gc.device->Swap(currentBuffer, NULL);

  currentBufferIdx++;
  currentBufferIdx = currentBufferIdx % MAX_BUFFERS;
};

bool Shutdown(GraphicsContext &graphicsContext) { return true; };

} // namespace Graphics