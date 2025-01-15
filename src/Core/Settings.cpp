#include "Settings.hpp"
#include "Common/FileSystem.hpp"
#include "Common/FileUtil.hpp"
#include "Common/Log.hpp"

Settings::Settings() {
  XVIDEO_MODE VideoMode;
  XGetVideoMode(&VideoMode);

  displayWidth = VideoMode.dwDisplayHeight;
  displayHeight = VideoMode.dwDisplayHeight;
  displayRefreshRate = VideoMode.RefreshRate;
  displayAspectRatio = (float)displayWidth / (float)displayHeight;

  renderAspectRatio = 4.0f/3.0f;
  internalResolutionScale = 1;
  enableVsync = false;

  Log::GenericLog("Created and loaded default settings");
}

Settings LoadDefaultSettings() { return Settings(); }

/* TODO: ini file loading/parsing

Settings ParseAndLoad(const char *filename) { return Settings(); }

Settings LoadSettings() {
  if (File::PathExists("settings.ini")) {
    return ParseAndLoad("settings.ini");
  }

  // TODO: show a messeage box warning the user
  // that no settings file was found
  // and we gonna create a default file on the xex dir

  // TODO: read this info and populate the settings struct
  
  XVIDEO_MODE VideoMode;
  ZeroMemory(&VideoMode, sizeof(VideoMode));
  XGetVideoMode(&VideoMode);
  

  return LoadDefaultSettings();
}

*/