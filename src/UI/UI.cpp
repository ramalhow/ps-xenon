#include "UI.hpp"

#include "Common\Log.hpp"
#include "MainScene.hpp"

#include <assert.h>

CMyApp app;

HRESULT CMyApp::RegisterXuiClasses() {
  HRESULT hr = CMainMenuScene::Register();
  if (FAILED(hr))
    return hr;
  hr = COsdMenuScene::Register();
  if (FAILED(hr))
    return hr;

  hr = CLoadStates::Register();
  if (FAILED(hr))
    return hr;

  hr = CSelectFilter::Register();
  if (FAILED(hr))
    return hr;

  hr = CDiscSwap::Register();
  if (FAILED(hr))
    return hr;

  hr = CGameGuide::Register();
  if (FAILED(hr))
    return hr;

  hr = CGameGuideV2::Register();
  if (FAILED(hr))
    return hr;

  hr = CXuiFileBrowser::Register();
  if (FAILED(hr))
    return hr;

  hr = CFileBrowserList::Register();
  if (FAILED(hr))
    return hr;

  hr = CLoadStateBrowser::Register();
  if (FAILED(hr))
    return hr;

  hr = CDiscSwapBrowser::Register();
  if (FAILED(hr))
    return hr;

  hr = CEffectBrowser::Register();
  if (FAILED(hr))
    return hr;

  return S_OK;
}

HRESULT CMyApp::UnregisterXuiClasses() {
  CMainMenuScene::Unregister();

  COsdMenuScene::Unregister();
  CLoadStates::Unregister();
  CSelectFilter::Unregister();
  CDiscSwap::Unregister();
  CGameGuide::Unregister();
  CXuiFileBrowser::Unregister();
  CFileBrowserList::Unregister();
  CLoadStateBrowser::Unregister();
  CDiscSwapBrowser::Unregister();
  CEffectBrowser::Unregister();
  CGameGuideV2::Unregister();

  return S_OK;
}

namespace UI {

UIContext Init(Settings &settings, GraphicsContext &graphicsContext) {
  UIContext uc;

  HRESULT hr = app.InitShared(graphicsContext.device,
                                 &graphicsContext.presentParameters,
                                 XuiD3DXTextureLoader, nullptr);
  assert(SUCCEEDED(hr));
  Log::GenericLog("XUI: app initialized");

  hr = app.RegisterDefaultTypeface(
      L"Arial Unicode MS",
      L"file://game:/media/PsxSkin.xzp#media/Graphics/xarialuni.ttf");

  assert(SUCCEEDED(hr));
  Log::GenericLog("XUI: Register a default typeface");

  hr = app.LoadSkin(
      L"file://game:/media/PsxSkin.xzp#media/Graphics/simple_scene_skin.xur");

  assert(SUCCEEDED(hr));
  Log::GenericLog("XUI: loaded skin");

  XuiSceneCreate(L"file://game:/media/PsxSkin.xzp#media/Graphics/",
                 L"scene.xur", NULL, &uc.mainSceneHandle);

  assert(SUCCEEDED(hr));
  Log::GenericLog("XUI: created scene");

  // Load the scene.
  hr = app.LoadFirstScene(L"file://game:/media/PsxSkin.xzp#media/Graphics/",
                             L"scene.xur", NULL);

  //assert(SUCCEEDED(hr));
  Log::GenericLog("XUI: loaded first scene");

  /* No threads for now
  // Start the in game thread
  HANDLE hInGameThread =
      CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)InGameThread, NULL,
                   CREATE_SUSPENDED, NULL);

  XSetThreadProcessor(hInGameThread, 5);
  ResumeThread(hInGameThread);

  */

  /* No need for view transforms
  // Set matrice for xui - allow to resize move correctly in small tvs
  D3DXMATRIX mat;
  D3DXMATRIX tr;

  XuiRenderGetViewTransform(app.GetDC(), &mat);
  D3DXMatrixTranslation(&tr, 0,
                        (((float)g_d3dpp.BackBufferHeight - 720.f) / 2.f), 0);
  D3DXMatrixMultiply(&mat, &mat, &tr);
  XuiRenderSetViewTransform(app.GetDC(), &mat);
  */

  return uc;
};

void Shutdown(UIContext &uiContext) {
  app.Uninit();
  Log::GenericLog("XUI: freed all resoures");
};
} // namespace UI