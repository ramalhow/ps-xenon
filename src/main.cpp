#include "Common/CommonXbox.h"

#include "Common/FileSystem.hpp"
#include "Common/Log.hpp"
#include "Core/Graphics.hpp"
#include "Core/Settings.hpp"
#include "UI/UI.hpp"

void __cdecl main() {

  FileSystem::Init();

  // TODO: implement debug log

  /* TODO: file loading code
  gSettings = LoadSettings();

  if (gSettings.firstBoot()) {
      CheckConfigFolders();
      CheckResources();
  }
  */

  Settings gSettings = LoadDefaultSettings();

  auto graphicsContext = Graphics::Init(gSettings);

  Graphics::LoadShaders(graphicsContext);

  auto uiContext = UI::Init(gSettings, graphicsContext);

  bool shouldExit = false;
  XINPUT_STATE InputState;
  while (!shouldExit) {

    graphicsContext.device->Clear(
        0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
        0xff000000, 1.0f, 0L);

    XInputGetState(0, &InputState);

    if (InputState.Gamepad.wButtons & XINPUT_GAMEPAD_A)
      Log::GenericLog("heyyy");

    app.RunFrame();
    app.Render();
    XuiTimersRun();

    Graphics::SwapBuffers(graphicsContext);

    graphicsContext.device->Present(nullptr, nullptr, nullptr, nullptr);
  }

  UI::Shutdown(uiContext);

  Graphics::Shutdown(graphicsContext);

  // TODO: implement SaveSettings()

  // Goodbye!
  FileSystem::Shutdown();
  XLaunchNewImage(XLAUNCH_KEYWORD_DASH, 0);
}