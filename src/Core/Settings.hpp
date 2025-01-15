#pragma once
#include <Common\CommonXbox.h>
#include <Common\Types.h>


struct Settings {
  explicit Settings();

  // display info
  uint16 displayWidth;
  uint16 displayHeight;
  float displayRefreshRate;
  float displayAspectRatio;

  /* TODO: handle this later
  bool displayIsHD;
  bool displayIsWidescreen;
  */

  // render settings
  float renderAspectRatio;
  uint8 internalResolutionScale;
  bool enableVsync;

};

Settings LoadDefaultSettings();

Settings LoadSettings();
