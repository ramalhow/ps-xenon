#pragma once

#include <Core/Graphics.hpp>
#include <Core/Settings.hpp>

#include <xuiapp.h>

class CMyApp : public CXuiModule {
protected:
  virtual HRESULT RegisterXuiClasses();
  virtual HRESULT UnregisterXuiClasses();
};

extern CMyApp app;

struct UIContext {
  HXUIOBJ mainSceneHandle;
};

namespace UI {

UIContext Init(Settings &settings, GraphicsContext &graphicsContext);

void UpdateXUI(UIContext &uiContext);

void RenderXUI(UIContext &uiContext);

void Shutdown(UIContext &uiContext);
} // namespace UI