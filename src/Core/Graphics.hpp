#pragma once

#include "Settings.hpp"

#define D3DCOMPILE_USEVOIDS
#include <d3d9.h>
#include <d3dx9.h>

struct GraphicsContext {
  explicit GraphicsContext();

  IDirect3D9 *d3d;
  IDirect3DDevice9 *device;
  D3DPRESENT_PARAMETERS presentParameters;
};

namespace Graphics {

GraphicsContext Init(Settings &settings);

void LoadShaders(GraphicsContext &gc);

void SwapBuffers(GraphicsContext &gc);

bool Shutdown(GraphicsContext &graphicsContext);

} // namespace Graphics