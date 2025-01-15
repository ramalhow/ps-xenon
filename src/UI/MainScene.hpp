#pragma once

#include <Common/CommonXbox.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <xuiapp.h>

/*
NOTE: this is very bad and confusing UI code, but somehow it works relatively
right in the original project.. so for now I just need to get this working as
before, then I can throw everything away and rebuild from scratch :)
*/


std::string get_string(const std::wstring &s, std::string &d) {
  char buffer[256];
  const wchar_t *cs = s.c_str();
  wcstombs(buffer, cs, sizeof(buffer));

  d = buffer;

  return d;
}

std::wstring get_wstring(const std::string &s, std::wstring &d) {
  wchar_t buffer[256];
  const char *cs = s.c_str();
  mbstowcs(buffer, cs, sizeof(buffer));

  d = buffer;

  return d;
}

std::wstring string2wstring(const std::string &s) {
  int len;
  int slength = (int)s.length() + 1;
  len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
  wchar_t *buf = new wchar_t[len];
  MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
  std::wstring r(buf);
  delete[] buf;
  return r;
}

class FileBrowserProvider {
protected:
  struct _FILE_INFO {
    std::wstring filename;
    std::wstring displayname;
    std::wstring gamecover;
    bool isDir;
  };

  std::wstring currentDir;
  std::vector<_FILE_INFO> fileList;

protected:
  static bool compare(const _FILE_INFO a, const _FILE_INFO b) {

    /* If one is a file and one is a directory the directory is first. */
    if (a.isDir && !b.isDir)
      return true;
    if (!a.isDir && b.isDir)
      return false;

    return a.displayname < b.displayname;
  }

  void Sort() { std::sort(fileList.begin(), fileList.end(), compare); }

public:
  void Init() { currentDir = L"game:"; }

  bool endsWith(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
      return (0 == fullString.compare(fullString.length() - ending.length(),
                                      ending.length(), ending));
    } else {
      return false;
    }
  }

  bool startWith(std::string const &fullstring, char const &filter) {
    if (fullstring[0] == filter)
      return true;
    else
      return false;
  }

  void AddParentEntry(std::wstring currentDir) {
    _FILE_INFO finfo;
    wchar_t lastLetter = currentDir[currentDir.length() - 1];
    if (lastLetter == L':') {
      finfo.isDir = true;
      finfo.displayname = L"DeviceList";
      finfo.filename = L"/";
      finfo.gamecover =
          L"file://game:/media/PsxSkin.xzp#media\\Graphics\\hdd.png";

      fileList.push_back(finfo);
      return;
    }

    unsigned int p = currentDir.rfind(L"\\");
    currentDir = currentDir.substr(0, p);

    finfo.isDir = true;
    finfo.displayname = L"..";
    finfo.gamecover =
        L"file://game:/media/PsxSkin.xzp#media\\Graphics\\back.png";

    finfo.filename = currentDir;

    fileList.push_back(finfo);
  }

  HRESULT AddDevicesList() {
    // Free file list
    fileList.clear();

    _FILE_INFO finfo;
    finfo.isDir = true;

    finfo.filename = L"game:";
    finfo.displayname = L"[GAME]";
    finfo.gamecover =
        L"file://game:/media/PsxSkin.xzp#media\\Graphics\\psx.jpg";
    fileList.push_back(finfo);

    finfo.filename = L"usb0:";
    finfo.displayname = L"[USB0]";
    finfo.gamecover =
        L"file://game:/media/PsxSkin.xzp#media\\Graphics\\usb.png";
    fileList.push_back(finfo);

    finfo.filename = L"usb1:";
    finfo.displayname = L"[USB1]";
    finfo.gamecover =
        L"file://game:/media/PsxSkin.xzp#media\\Graphics\\usb.png";
    fileList.push_back(finfo);

    finfo.filename = L"hdd0:";
    finfo.displayname = L"[HDD0]";
    finfo.gamecover =
        L"file://game:/media/PsxSkin.xzp#media\\Graphics\\hdd.png";
    fileList.push_back(finfo);

    finfo.filename = L"hdd1:";
    finfo.displayname = L"[HDD1]";
    finfo.gamecover =
        L"file://game:/media/PsxSkin.xzp#media\\Graphics\\hdd.png";
    fileList.push_back(finfo);

    finfo.filename = L"hdd2:";
    finfo.displayname = L"[HDD2]";
    finfo.gamecover =
        L"file://game:/media/PsxSkin.xzp#media\\Graphics\\hdd.png";
    fileList.push_back(finfo);

    finfo.filename = L"hdd3:";
    finfo.displayname = L"[HDD3]";
    finfo.gamecover =
        L"file://game:/media/PsxSkin.xzp#media\\Graphics\\hdd.png";
    fileList.push_back(finfo);

    return S_OK;
  }

  HRESULT UpdateDirList(std::wstring currentDir = L"game:\\roms") {
    WIN32_FIND_DATA ffd;
    std::string szDir;

    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;

    if (currentDir == L"/") {
      AddDevicesList();
      return S_OK;
    }
    // Free file list
    fileList.clear();
    AddParentEntry(currentDir);

    get_string(currentDir, szDir);
    szDir = szDir + "\\*";

    // Look for files in current directory
    hFind = FindFirstFile(szDir.c_str(), &ffd);

    if (INVALID_HANDLE_VALUE == hFind) {
      return S_FALSE;
    }

    // List all the files in the directory with some info about them.
    do {
      _FILE_INFO finfo;
      finfo.isDir =
          (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
      get_wstring(ffd.cFileName, finfo.displayname);
      finfo.filename = currentDir + L"\\" + finfo.displayname;

      // :s

      // teste adding cover into scrool items
      if (finfo.isDir) {
        finfo.displayname = L"[" + finfo.displayname + L"]";
        finfo.gamecover =
            L"file://game:/media/PsxSkin.xzp#media\\Graphics\\folder.png";
      } else {
        std::string coverpath =
            "game:\\covers\\" +
            std::string(finfo.displayname.begin(), finfo.displayname.end());

        coverpath.append(".jpg");
        /*
        if (fileExists((char *)coverpath.c_str()))
  finfo.gamecover =
      L"file://game:\\covers\\" + finfo.displayname + L".jpg";
else

        */
        finfo.gamecover = L"file://game:/media/PsxSkin.xzp#media\\psx.jpg";
      }

      if (true) {

        // ext <- rom extension check
        std::string ext(finfo.displayname.begin(), finfo.displayname.end());
        if (endsWith(ext, ".img"))
          fileList.push_back(finfo);
        else if (endsWith(ext, ".bin"))
          fileList.push_back(finfo);
        else if (endsWith(ext, ".mdf"))
          fileList.push_back(finfo);
        else if (endsWith(ext, ".iso"))
          fileList.push_back(finfo);
        else if (endsWith(ext, ".IMG"))
          fileList.push_back(finfo);
        else if (endsWith(ext, ".BIN"))
          fileList.push_back(finfo);
        else if (endsWith(ext, ".MDF"))
          fileList.push_back(finfo);
        else if (endsWith(ext, ".ISO"))
          fileList.push_back(finfo);
        else if (finfo.isDir)
          fileList.push_back(finfo);
      } else
        fileList.push_back(finfo);

    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);

    Sort();

    return S_OK;
  }

  // �pdateDirList filter mode
  HRESULT UpdateDirListFilterMode(std::wstring currentDir = L"game:\\roms",
                                  char letter = (char)"") {
    WIN32_FIND_DATA ffd;
    std::string szDir;

    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;

    if (currentDir == L"/") {
      AddDevicesList();
      return S_OK;
    }
    // Free file list
    fileList.clear();
    AddParentEntry(currentDir);

    get_string(currentDir, szDir);
    szDir = szDir + "\\*";

    // Look for files in current directory
    hFind = FindFirstFile(szDir.c_str(), &ffd);

    if (INVALID_HANDLE_VALUE == hFind) {
      return S_FALSE;
    }

    // List all the files in the directory with some info about them.
    do {
      _FILE_INFO finfo;
      finfo.isDir =
          (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
      get_wstring(ffd.cFileName, finfo.displayname);
      finfo.filename = currentDir + L"\\" + finfo.displayname;

      // :s

      // teste adding cover into scrool items
      if (finfo.isDir) {
        finfo.displayname = L"[" + finfo.displayname + L"]";
        finfo.gamecover =
            L"file://game:/media/PsxSkin.xzp#media\\Graphics\\folder.png";
      } else {
        std::string coverpath =
            "game:\\covers\\" +
            std::string(finfo.displayname.begin(), finfo.displayname.end());

        coverpath.append(".jpg");

        /*
                if (fileExists((char *)coverpath.c_str()))
          finfo.gamecover =
              L"file://game:\\covers\\" + finfo.displayname + L".jpg";
        else
                */
        finfo.gamecover = L"file://game:/media/PsxSkin.xzp#media\\psx.jpg";
      }

      if (true) {

        // ext <- rom extension check
        std::string ext(finfo.displayname.begin(), finfo.displayname.end());
        if (startWith(ext, letter)) {

          if (endsWith(ext, ".img"))
            fileList.push_back(finfo);
          else if (endsWith(ext, ".bin"))
            fileList.push_back(finfo);
          else if (endsWith(ext, ".mdf"))
            fileList.push_back(finfo);
          else if (endsWith(ext, ".iso"))
            fileList.push_back(finfo);
          else if (endsWith(ext, ".IMG"))
            fileList.push_back(finfo);
          else if (endsWith(ext, ".BIN"))
            fileList.push_back(finfo);
          else if (endsWith(ext, ".MDF"))
            fileList.push_back(finfo);
          else if (endsWith(ext, ".ISO"))
            fileList.push_back(finfo);
          else if (finfo.isDir)
            fileList.push_back(finfo);

        } else
          fileList.push_back(finfo);
      } // apenas adiciona itens com letra

    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);

    Sort();

    return S_OK;
  }
  DWORD Size() { return this->fileList.size(); }

  LPCWSTR At(unsigned int i) {
    if (i >= 0 && i < Size()) {
      return this->fileList[i].displayname.c_str();
    } else {
      return L"";
    }
  }

  LPCWSTR GameCover(unsigned int i) {
    if (i >= 0 && i < Size()) {
      return this->fileList[i].gamecover.c_str();
    } else {
      return L"";
    }
  }

  LPCWSTR Filename(unsigned int i) {
    if (i >= 0 && i < Size()) {
      return this->fileList[i].filename.c_str();
    } else {
      return L"";
    }
  }

  bool IsDir(unsigned int i) {
    if (i >= 0 && i < Size()) {
      return this->fileList[i].isDir;
    } else {
      return false;
    }
  }
};

FileBrowserProvider fileList;
FileBrowserProvider saveStateList;
FileBrowserProvider effectList;
FileBrowserProvider swapdiscList;
FileBrowserProvider gameguideList;

class CMainMenuScene : public CXuiSceneImpl {

protected:
  wchar_t fileBrowserInfoText[512];
  bool coverModeHorizontal;

  CXuiList FileBrowser;
  CXuiList FileBrowserV;
  CXuiNavButton OsdBtn;
  CXuiControl FileBrowserInfo;

  CXuiCheckbox DynarecCbox;
  CXuiCheckbox GpuThCbox;
  CXuiCheckbox SpuIrqCbox;
  CXuiCheckbox FrameLimitCbox;
  CXuiCheckbox ParasiteEveFixCbox;
  CXuiCheckbox DarkForcesFixCbox;
  CXuiCheckbox slowbootCbox;
  CXuiCheckbox LinearFilterCbox;
  CXuiCheckbox cpubiasCbox;
  CXuiCheckbox TombRaiderCbox;
  CXuiCheckbox FrontMissionCbox;

  CXuiControl txtteste;
  CXuiControl txtTitleEmu;
  CXuiNavButton Backbtn;
  CXuiImageElement imgCover;
  CXuiNavButton ChangeCoverMode;
  CXuiNavButton BtnProfileSet;
  CXuiControl btnSaveProfile;
  CXuiControl bgcolorProfile;
  CXuiControl ExitBtn;
  CXuiControl txtGameProfileInfo;

  XUI_BEGIN_MSG_MAP()
  XUI_ON_XM_INIT(OnInit)
  XUI_ON_XM_NOTIFY_PRESS(OnNotifyPress)
  XUI_ON_XM_RENDER(OnRender)

  XUI_END_MSG_MAP()

  //----------------------------------------------------------------------------------
  // Performs initialization tasks - retrieves controls.
  //----------------------------------------------------------------------------------
  HRESULT OnInit(XUIMessageInit *pInitData, BOOL &bHandled) {
    GetChildById(L"FileBrowser", &FileBrowser);
    GetChildById(L"FileBrowserV", &FileBrowserV);
    GetChildById(L"OsdBtn", &OsdBtn);
    GetChildById(L"FileBrowserInfo", &FileBrowserInfo);

    GetChildById(L"DynarecCbox", &DynarecCbox);
    GetChildById(L"GpuThCbox", &GpuThCbox);
    GetChildById(L"SpuIrqCbox", &SpuIrqCbox);
    GetChildById(L"FrameLimitCbox", &FrameLimitCbox);
    GetChildById(L"ParasiteEveFixCbox", &ParasiteEveFixCbox);
    GetChildById(L"DarkForcesFixCbox", &DarkForcesFixCbox);
    GetChildById(L"slowbootCbox", &slowbootCbox);
    GetChildById(L"LinearFilterCbox", &LinearFilterCbox);
    GetChildById(L"cpubiasCbox", &cpubiasCbox);
    GetChildById(L"TombRaiderCbox", &TombRaiderCbox);
    GetChildById(L"FrontMissionCbox", &FrontMissionCbox);

    GetChildById(L"GitVersion", &txtTitleEmu);
    GetChildById(L"XuiLabel3", &txtteste);
    GetChildById(L"txtGameProfileInfo", &txtGameProfileInfo);

    GetChildById(L"Backbtn", &Backbtn);
    GetChildById(L"PsxScreen", &imgCover);
    GetChildById(L"BtnCoverMode", &ChangeCoverMode);
    GetChildById(L"BtnProfileSet", &BtnProfileSet);
    GetChildById(L"btnSaveProfile", &btnSaveProfile);
    GetChildById(L"bgcolorProfile", &bgcolorProfile);
    GetChildById(L"ExitBtn", &ExitBtn);

    txtteste.SetShow(false);
    txtTitleEmu.SetText(
        L"PCSXR-360 Release 2.1.1 by Dreamboy - Special thanks go to ced2911 & "
        L"cmkn1983 & Swizzy for internal coding");
    txtGameProfileInfo.SetShow(false);

    // Init values from xbox config
    DynarecCbox.SetCheck(false);
    GpuThCbox.SetCheck(false);
    SpuIrqCbox.SetCheck(false);
    FrameLimitCbox.SetCheck(false);
    ParasiteEveFixCbox.SetCheck(false);
    DarkForcesFixCbox.SetCheck(false);
    slowbootCbox.SetCheck(false);
    LinearFilterCbox.SetCheck(false);
    cpubiasCbox.SetCheck(false);
    TombRaiderCbox.SetCheck(false);
    FrontMissionCbox.SetCheck(false);

    coverModeHorizontal = false;

    /*
    if (xboxConfig.noGameBrowse) {
      // Check for bios file wip
      if ((xboxConfig.InfoboxShow) && (xboxConfig.Infomsg != L"")) {
        LPCWSTR msgtype;
        if (xboxConfig.InfoBoxType == 1)
          msgtype = L"PCSXR360 Info";
        else if (xboxConfig.InfoBoxType == 2)
          msgtype = L"PCSXR360 System Error";

        ShowMessageBoxEx(NULL, NULL, msgtype, xboxConfig.Infomsg, 1,
                         (LPCWSTR *)L"OK", NULL, XUI_MB_CENTER_ON_PARENT, NULL);
        // bHandled = TRUE;
        // return xex
        XLaunchNewImage("", NULL);
      }

      OsdBtn.Press();
    }
    */

    return S_OK;
  }

  HRESULT OnRender(XUIMessageRender *pInputData, BOOL &bHandled) {

    // imgCover.SetImagePath(&xboxConfig.CoverPath[0]);

    swprintf(fileBrowserInfoText, L"%d/%d", FileBrowser.GetCurSel() + 1,
             FileBrowser.GetItemCount());
    FileBrowserInfo.SetText(fileBrowserInfoText);

    return S_OK;
  }

  HRESULT OnNotifyPress(HXUIOBJ hObjPressed, BOOL &bHandled) {

    if (hObjPressed == FileBrowser) {
      // xboxConfig.noGameBrowse = TRUE;
      // LoadSettings();
      OsdBtn.Press();
    }
    if (hObjPressed == FileBrowserV) {
      // xboxConfig.noGameBrowse = TRUE;
      // LoadSettings();
      OsdBtn.Press();
    }

    if (hObjPressed == DynarecCbox) {
      // xboxConfig.UseDynarec = DynarecCbox.IsChecked();
    }

    if (hObjPressed == GpuThCbox) {
      // xboxConfig.UseThreadedGpu = GpuThCbox.IsChecked();
    }

    if (hObjPressed == SpuIrqCbox) {
      // xboxConfig.UseSpuIrq = SpuIrqCbox.IsChecked();
    }

    if (hObjPressed == FrameLimitCbox) {
      // xboxConfig.UseFrameLimiter = FrameLimitCbox.IsChecked();
    }

    if (hObjPressed == ParasiteEveFixCbox) {
      // xboxConfig.UseParasiteEveFix = ParasiteEveFixCbox.IsChecked();
    }

    if (hObjPressed == DarkForcesFixCbox) {
      // xboxConfig.UseDarkForcesFix = DarkForcesFixCbox.IsChecked();
    }

    if (hObjPressed == slowbootCbox) {
      // xboxConfig.UseSlowBoot = slowbootCbox.IsChecked();
    }

    if (hObjPressed == LinearFilterCbox) {
      // xboxConfig.UseLinearFilter = LinearFilterCbox.IsChecked();
    }
    if (hObjPressed == cpubiasCbox) {
      // xboxConfig.UseCpuBias = cpubiasCbox.IsChecked();
    }

    if (hObjPressed == TombRaiderCbox) {
      // xboxConfig.UseTombRaider2Fix = TombRaiderCbox.IsChecked();
    }

    if (hObjPressed == FrontMissionCbox) {
      // xboxConfig.UseFrontMission3Fix = FrontMissionCbox.IsChecked();
    }

    if (hObjPressed == Backbtn) {

      // test fexa profile menu caso se faxa backbtn  Marco
      txtGameProfileInfo.SetShow(FALSE);
      bgcolorProfile.SetShow(FALSE);
      bgcolorProfile.SetEnable(FALSE);
      LinearFilterCbox.SetShow(FALSE);
      DynarecCbox.SetShow(FALSE);
      GpuThCbox.SetShow(FALSE);
      SpuIrqCbox.SetShow(FALSE);
      FrameLimitCbox.SetShow(FALSE);
      ParasiteEveFixCbox.SetShow(FALSE);
      DarkForcesFixCbox.SetShow(FALSE);
      slowbootCbox.SetShow(FALSE);
      TombRaiderCbox.SetShow(FALSE);
      FrontMissionCbox.SetShow(FALSE);
      cpubiasCbox.SetShow(FALSE);
      SpuIrqCbox.SetShow(FALSE);
      btnSaveProfile.SetShow(FALSE);

      //--------------------------------------------------------------
    }

    if (hObjPressed == ExitBtn) {

      XLaunchNewImage("", NULL);
    }

    if (hObjPressed == ChangeCoverMode) {

      coverModeHorizontal = !coverModeHorizontal;
      FileBrowser.SetShow(coverModeHorizontal);
      FileBrowser.SetEnable(coverModeHorizontal);
      imgCover.SetShow(!coverModeHorizontal);
      FileBrowserV.SetShow(!coverModeHorizontal);
      FileBrowserV.SetEnable(!coverModeHorizontal);

      if (coverModeHorizontal) {
        FileBrowser.SetCurSelVisible(FileBrowserV.GetCurSel());
        FileBrowser.SetFocus();
      } else {
        FileBrowserV.SetCurSelVisible(FileBrowser.GetCurSel());
        FileBrowserV.SetFocus();
      }
      bHandled = TRUE;
    }

    if (hObjPressed == BtnProfileSet) {

      // char * path= 0;
      // LoadSettings();

      // Set checkbox with right configs from game profile  @Dreamboy 23/12/2016

      /*
  LinearFilterCbox.SetCheck(false);
  DynarecCbox.SetCheck(xboxConfig.UseDynarec);
  GpuThCbox.SetCheck(xboxConfig.UseThreadedGpu);
  SpuIrqCbox.SetCheck(xboxConfig.UseSpuIrq);
  FrameLimitCbox.SetCheck(xboxConfig.UseFrameLimiter);
  ParasiteEveFixCbox.SetCheck(xboxConfig.UseParasiteEveFix);
  DarkForcesFixCbox.SetCheck(xboxConfig.UseDarkForcesFix);
  slowbootCbox.SetCheck(xboxConfig.UseSlowBoot); // psx bios boot
  TombRaiderCbox.SetCheck(xboxConfig.UseTombRaider2Fix);
  FrontMissionCbox.SetCheck(xboxConfig.UseFrontMission3Fix);
  cpubiasCbox.SetCheck(xboxConfig.UseCpuBias);
      */
      txtGameProfileInfo.SetText(L"game name placeholder");

      txtGameProfileInfo.SetShow(TRUE);
      bgcolorProfile.SetShow(TRUE);
      bgcolorProfile.SetEnable(TRUE);
      LinearFilterCbox.SetShow(TRUE);
      DynarecCbox.SetShow(TRUE);
      GpuThCbox.SetShow(TRUE);
      SpuIrqCbox.SetShow(TRUE);
      FrameLimitCbox.SetShow(TRUE);
      ParasiteEveFixCbox.SetShow(TRUE);
      DarkForcesFixCbox.SetShow(TRUE);
      slowbootCbox.SetShow(TRUE);
      TombRaiderCbox.SetShow(TRUE);
      FrontMissionCbox.SetShow(TRUE);
      cpubiasCbox.SetShow(TRUE);
      btnSaveProfile.SetShow(TRUE);
      LinearFilterCbox.SetFocus();

      bHandled = TRUE;
    }

    if (hObjPressed == btnSaveProfile) {
      char *path = 0;

      txtGameProfileInfo.SetShow(FALSE);
      bgcolorProfile.SetShow(FALSE);
      bgcolorProfile.SetEnable(FALSE);
      LinearFilterCbox.SetShow(FALSE);
      DynarecCbox.SetShow(FALSE);
      GpuThCbox.SetShow(FALSE);
      SpuIrqCbox.SetShow(FALSE);
      FrameLimitCbox.SetShow(FALSE);
      ParasiteEveFixCbox.SetShow(FALSE);
      DarkForcesFixCbox.SetShow(FALSE);
      slowbootCbox.SetShow(FALSE);
      TombRaiderCbox.SetShow(FALSE);
      FrontMissionCbox.SetShow(FALSE);
      cpubiasCbox.SetShow(FALSE);
      SpuIrqCbox.SetShow(FALSE);
      btnSaveProfile.SetShow(FALSE);

      if (coverModeHorizontal)
        FileBrowser.SetFocus();
      else
        FileBrowserV.SetFocus();

      // ApplySettings("");
      // Sleep(1);
      // LoadSettings();

      // std::wstring wpath = xboxConfig.GamePath;
      const WCHAR *button_text = L"OK";
      ShowMessageBoxEx(NULL, NULL, L"Game Profile Settings Saved",
                       L"game name placeholder", 1, (LPCWSTR *)&button_text,
                       NULL, XUI_MB_CENTER_ON_PARENT, NULL);
      bHandled = TRUE;
    }

    return S_OK;
  }

public:
  XUI_IMPLEMENT_CLASS(CMainMenuScene, L"MainMenu", XUI_CLASS_SCENE)
};

class COsdMenuScene : public CXuiSceneImpl {
protected:
  std::string game;
  std::wstring wgame;

  CXuiControl BackBtn;
  CXuiControl ResetBtn;
  CXuiControl SelectBtn;
  CXuiControl btnBack;
  CXuiImageElement xuiGameImg;
  CXuiControl SaveStateBtn;
  CXuiControl GuidebtnHTML;
  CXuiList RegionList;

  COsdMenuScene() {}

  ~COsdMenuScene() {}

  XUI_BEGIN_MSG_MAP()
  XUI_ON_XM_INIT(OnInit)
  XUI_ON_XM_NOTIFY_PRESS(OnNotifyPress)
  XUI_ON_XM_RENDER(OnRenderOSD)
  XUI_ON_XM_NOTIFY_KILL_FOCUS(OnNotifyKillFocus)
  XUI_END_MSG_MAP()

  //----------------------------------------------------------------------------------
  // Performs initialization tasks - retrieves controls.
  //----------------------------------------------------------------------------------
  HRESULT OnInit(XUIMessageInit *pInitData, BOOL &bHandled) {
    GetChildById(L"BackBtn", &BackBtn);
    GetChildById(L"ResetBtn", &ResetBtn);
    GetChildById(L"SelectBtn", &SelectBtn);
    GetChildById(L"btnBack", &btnBack);
    GetChildById(L"GuidebtnHTML", &GuidebtnHTML);

    GetChildById(L"SaveStateBtn", &SaveStateBtn);
    GetChildById(L"RegionList", &RegionList);
    GetChildById(L"xuiGameImg", &xuiGameImg);
    // xboxConfig.noGameBrowse = TRUE;
    effectList.UpdateDirList(L"game:\\hlsl");

    // RegionList.SetCurSelVisible(xboxConfig.region);

    // xuiGameImg.SetImagePath(&xboxConfig.CoverPath[0]);

    return S_OK;
  }

  HRESULT OnNotifyKillFocus(HXUIOBJ hObjSource,
                            XUINotifyFocus *pNotifyFocusData, BOOL &bHandled) {

    /*
    if (hObjSource == RegionList) {
xboxConfig.region = RegionList.GetCurSel();
}
  */

    return S_OK;
  }

  HRESULT OnRenderOSD(XUIMessageRender *pInputData, BOOL &bHandled) {

    return S_OK;
  }

  HRESULT OnNotifyPress(HXUIOBJ hObjPressed, BOOL &bHandled) {

    if (hObjPressed == BackBtn) {
      // xboxConfig.noGameBrowse = TRUE;
      // ResumePcsx(); // test
      bHandled = TRUE;
    }

    if (hObjPressed == ResetBtn) {
      // xboxConfig.noGameBrowse = TRUE;
      // ResetPcsx();
    }

    if (hObjPressed == SelectBtn) {
      // xboxConfig.game.clear();
      // xboxConfig.noGameBrowse = FALSE;
      // ShutdownPcsx();
      // xboxConfig.Running = false;
      NavigateBack();
      // LoadSettings();
      bHandled = TRUE;
    }

    // show
    if (hObjPressed == SaveStateBtn) {
      // xboxConfig.noGameBrowse = FALSE;
      // SaveStatePcsx(-1);

      bHandled = TRUE;
    }

    if (hObjPressed == btnBack) {
      // xboxConfig.noGameBrowse = TRUE;
      // ResumePcsx();
      bHandled = TRUE;
    }

    return S_OK;
  }

public:
  XUI_IMPLEMENT_CLASS(COsdMenuScene, L"InGameMenu", XUI_CLASS_SCENE)
};

class CLoadStates : public CXuiSceneImpl {
protected:
  CXuiList LoadStateBrowser;

  XUI_BEGIN_MSG_MAP()
  XUI_ON_XM_INIT(OnInit)
  XUI_END_MSG_MAP()

  //----------------------------------------------------------------------------------
  // Performs initialization tasks - retrieves controls.
  //----------------------------------------------------------------------------------
  HRESULT OnInit(XUIMessageInit *pInitData, BOOL &bHandled) {

    GetChildById(L"LoadStateBrowser", &LoadStateBrowser);

    // path

    /*
std::string path = xboxConfig.game;
path.erase(0, path.rfind('\\') + 1);

std::string saveStateDir = xboxConfig.saveStateDir + "\\" + path;
std::wstring wg;
get_wstring(saveStateDir, wg);

    */
    saveStateList.UpdateDirList(L"game:/");

    return S_OK;
  }

public:
  XUI_IMPLEMENT_CLASS(CLoadStates, L"LoadStates", XUI_CLASS_SCENE)
};

class CSelectFilter : public CXuiSceneImpl {
protected:
  CXuiList EffectList;

  XUI_BEGIN_MSG_MAP()
  XUI_ON_XM_INIT(OnInit)
  XUI_END_MSG_MAP()

  //----------------------------------------------------------------------------------
  // Performs initialization tasks - retrieves controls.
  //----------------------------------------------------------------------------------
  HRESULT OnInit(XUIMessageInit *pInitData, BOOL &bHandled) {

    GetChildById(L"EffectList", &EffectList);

    effectList.UpdateDirList(L"game:\\hlsl");

    return S_OK;
  }

public:
  XUI_IMPLEMENT_CLASS(CSelectFilter, L"SelectFilter", XUI_CLASS_SCENE)
};

class CDiscSwap : public CXuiSceneImpl {
protected:
  CXuiList DiscSwapBrowser;

  XUI_BEGIN_MSG_MAP()
  XUI_ON_XM_INIT(OnInit)
  XUI_END_MSG_MAP()

  //----------------------------------------------------------------------------------
  // Performs initialization tasks - retrieves controls.
  //----------------------------------------------------------------------------------
  HRESULT OnInit(XUIMessageInit *pInitData, BOOL &bHandled) {

    GetChildById(L"DiscSwapBrowser", &DiscSwapBrowser);
    swapdiscList.UpdateDirList(L"game:\\roms");

    return S_OK;
  }

public:
  XUI_IMPLEMENT_CLASS(CDiscSwap, L"DiscSwap", XUI_CLASS_SCENE)
};

float txtspeed = 130;
float txtaccel = 12;

class CGameGuide : public CXuiSceneImpl {

protected:
  // Control and Element wrapper objects.
  CXuiTextElement m_RendererText;
  CXuiEdit m_Edit;
  CXuiHtmlControl m_HtmlCode;
  // Message map.
  XUI_BEGIN_MSG_MAP()
  XUI_ON_XM_INIT(OnInit)
  XUI_END_MSG_MAP()

  //----------------------------------------------------------------------------------
  // Performs initialization tasks - retreives controls.
  //----------------------------------------------------------------------------------
  HRESULT OnInit(XUIMessageInit *pInitData, BOOL &bHandled) {

    // Retrieve controls for later use.
    GetChildById(L"RendererText", &m_RendererText);
    GetChildById(L"TextEdit", &m_Edit);
    GetChildById(L"XuiHtml", &m_HtmlCode);

    /*
gameguide = xboxConfig.game;
// Get basename of current game
gameguide.erase(0, gameguide.rfind('\\') + 1);
gameguide = "game:\\gameguides\\" + gameguide;
gameguide = gameguide + ".txt";
txtfile = ReadFile(gameguide);

    */

    m_RendererText.SetShow(TRUE);
    m_RendererText.SetFocus();
    // Init text field and edit control text

    m_RendererText.SetText(L"teste");
    m_Edit.SetVSmoothScroll(true, 100, txtaccel, txtspeed);
    m_RendererText.SetText(L"About The Game");

    return S_OK;
  }

public:
  XUI_IMPLEMENT_CLASS(CGameGuide, L"GameGuide", XUI_CLASS_SCENE)
};

//----------------------------------------------------------------

//------------------Gameguide V2 HTML Mode ON bitch------------------
class CGameGuideV2 : public CXuiSceneImpl {

protected:
  // Control and Element wrapper objects.
  CXuiTextElement m_RendererText;

  // CXuiEdit m_Edit;
  CXuiHtmlElement m_xuiHtml;

  // Message map.
  XUI_BEGIN_MSG_MAP()
  XUI_ON_XM_INIT(OnInit)
  XUI_END_MSG_MAP()

  //----------------------------------------------------------------------------------
  // Performs initialization tasks - retreives controls.
  //----------------------------------------------------------------------------------
  HRESULT OnInit(XUIMessageInit *pInitData, BOOL &bHandled) {

    // Retrieve controls for later use.
    GetChildById(L"RendererText", &m_RendererText);
    GetChildById(L"XuiHtml", &m_xuiHtml); // html code

    /*
gameguide = xboxConfig.game;
// Get basename of current game
gameguide.erase(0, gameguide.rfind('\\') + 1);
gameguide = "game:\\gameguides\\" + gameguide;
gameguide = gameguide + ".html";
    */
    return S_OK;
  }

public:
  XUI_IMPLEMENT_CLASS(CGameGuide, L"GameGuideHTML", XUI_CLASS_SCENE)
};

//----------------------------------------------------------------

//----------------------------------------------------------------Clist

class CXuiFileBrowser : public CXuiListImpl {
protected:
  FileBrowserProvider *fileBrowser;

  // Message map. Here we tie messages to message handlers.
  XUI_BEGIN_MSG_MAP()
  XUI_ON_XM_GET_SOURCE_TEXT(OnGetSourceText)
  XUI_ON_XM_GET_ITEMCOUNT_ALL(OnGetItemCountAll)
  XUI_ON_XM_INIT(OnInit)
  XUI_ON_XM_SET_CURSEL(OnSetCurSel)
  XUI_ON_XM_GET_SOURCE_IMAGE(OnGetSourceDataImage)
  XUI_ON_XM_NOTIFY_PRESS(OnNotifyPress)
  XUI_END_MSG_MAP()

  //----------------------------------------------------------------------------------
  // Performs initialization tasks - retrieves controls.
  //----------------------------------------------------------------------------------
  HRESULT OnInit(XUIMessageInit *pInitData, BOOL &bHandled) {
    fileBrowser->Init();
    fileBrowser->UpdateDirList();

    return S_OK;
  }

  //----------------------------------------------------------------------------------
  // Returns the number of items in the list.
  //----------------------------------------------------------------------------------
  HRESULT OnGetItemCountAll(XUIMessageGetItemCount *pGetItemCountData,
                            BOOL &bHandled) {
    pGetItemCountData->cItems = fileBrowser->Size();
    bHandled = TRUE;
    return S_OK;
  }

  //----------------------------------------------------------------------------------
  // Returns the text for the items in the list.
  //----------------------------------------------------------------------------------
  HRESULT OnGetSourceText(XUIMessageGetSourceText *pGetSourceTextData,
                          BOOL &bHandled) {
    if (pGetSourceTextData->bItemData && pGetSourceTextData->iItem >= 0) {
      pGetSourceTextData->szText = fileBrowser->At(pGetSourceTextData->iItem);
      bHandled = TRUE;
    }
    return S_OK;
  }

  // Go get game cover
  HRESULT OnGetSourceDataImage(XUIMessageGetSourceImage *pGetSourceImageData,
                               BOOL &bHandled) {
    if ((0 == pGetSourceImageData->iData) && (pGetSourceImageData->bItemData)) {

      pGetSourceImageData->szPath =
          fileBrowser->GameCover(pGetSourceImageData->iItem);

      bHandled = TRUE;
    }
    return S_OK;
  }

  HRESULT OnSetCurSel(XUIMessageSetCurSel *pSetCurSelData, BOOL &bHandled) {

    std::string rompathcover = ""; //,y="",z="";
    std::string rompath = "";
    std::string romName = "";

    get_string(fileBrowser->GameCover(pSetCurSelData->iItem),
               rompathcover); // Full rom path ->Filename; display only romname
                              // use ->AT ;; Gamecover returns game cover path
                              // from the rom list :D hehe
    get_string(fileBrowser->Filename(pSetCurSelData->iItem), rompath);
    get_string(fileBrowser->Filename(pSetCurSelData->iItem), romName);

    /*
xboxConfig.CoverPath =
    std::wstring(rompathcover.begin(), rompathcover.end());
xboxConfig.GamePath = std::wstring(rompath.begin(), rompath.end());
xboxConfig.GameName = std::wstring(romName.begin(), romName.end());
    */

    // todo LoadGameProfile startup Marco 2017
    /*	ApplySettings("");
            Sleep(1);
            LoadSettings();
            */
    //------------------------------------

    bHandled = FALSE;
    return S_OK;
  }

  virtual HRESULT OnNotifyPress(HXUIOBJ hObjPressed, BOOL &bHandled) {

    // Change the current dir
    if (fileBrowser->IsDir(GetCurSel())) {
      // Delete old item count
      DeleteItems(0, fileBrowser->Size());

      // Update filelist
      fileBrowser->UpdateDirList(fileBrowser->Filename(GetCurSel()));

      // Insert item count
      InsertItems(0, fileBrowser->Size());

      // Move to top
      SetCurSelVisible(0);

      // Don't Notify parent
      bHandled = TRUE;

    } else {
      // get_string(fileBrowser->Filename(GetCurSel()), xboxConfig.game);

      // scene must done some work
      bHandled = FALSE;
    }
    return S_OK;
  }

public:
  // Define the class. The class name must match the ClassOverride property
  // set for the scene in the UI Authoring tool.
  XUI_IMPLEMENT_CLASS(CXuiFileBrowser, L"CXuiFileBrowser", XUI_CLASS_SCENE)
};

//------------------------------------------------------------------

//------------------------------------------------------------------Cxuifilebrowser

class CFileBrowserList : public CXuiFileBrowser {
protected:
  CFileBrowserList() { fileBrowser = &fileList; }

  virtual HRESULT OnNotifyPress(HXUIOBJ hObjPressed, BOOL &bHandled) {

    // Change the current dir
    if (fileBrowser->IsDir(GetCurSel())) {
      // Delete old item count
      DeleteItems(0, fileBrowser->Size());

      // Update filelist
      fileBrowser->UpdateDirList(fileBrowser->Filename(GetCurSel()));

      // Insert item count
      InsertItems(0, fileBrowser->Size());

      // Move to top
      SetCurSelVisible(0);

      // Don't Notify parent
      bHandled = TRUE;

    } else {
      // get_string(fileBrowser->Filename(GetCurSel()), xboxConfig.game);

      // scene must done some work
      bHandled = FALSE;
    }
    return S_OK;
  }

public:
  // Define the class. The class name must match the ClassOverride property
  // set for the scene in the UI Authoring tool.
  XUI_IMPLEMENT_CLASS(CFileBrowserList, L"FileBrowserList", XUI_CLASS_LIST)
};

class CLoadStateBrowser : public CXuiFileBrowser {
protected:
  CLoadStateBrowser() { fileBrowser = &saveStateList; }

  virtual HRESULT OnNotifyPress(HXUIOBJ hObjPressed, BOOL &bHandled) {

    // Change the current dir
    if (fileBrowser->IsDir(GetCurSel())) {
      // Delete old item count
      DeleteItems(0, fileBrowser->Size());

      // Update filelist
      fileBrowser->UpdateDirList(fileBrowser->Filename(GetCurSel()));

      // Insert item count
      InsertItems(0, fileBrowser->Size());

      // Move to top
      SetCurSelVisible(0);

      // Don't Notify parent
      bHandled = TRUE;

    } else {
      std::string path;
      get_string(fileBrowser->Filename(GetCurSel()), path);

      // LoadStatePcsx(path);
      bHandled = FALSE;
    }
    return S_OK;
  }

public:
  // Define the class. The class name must match the ClassOverride property
  // set for the scene in the UI Authoring tool.
  XUI_IMPLEMENT_CLASS(CLoadStateBrowser, L"LoadStateBrowser", XUI_CLASS_LIST)
};

class CDiscSwapBrowser : public CXuiFileBrowser {
protected:
  CDiscSwapBrowser() { fileBrowser = &swapdiscList; }

  virtual HRESULT OnNotifyPress(HXUIOBJ hObjPressed, BOOL &bHandled) {

    // Change the current dir
    if (fileBrowser->IsDir(GetCurSel())) {
      // Delete old item count
      DeleteItems(0, fileBrowser->Size());

      // Update filelist
      fileBrowser->UpdateDirList(fileBrowser->Filename(GetCurSel()));

      // Insert item count
      InsertItems(0, fileBrowser->Size());

      // Move to top
      SetCurSelVisible(0);

      // Don't Notify parent
      bHandled = TRUE;

    } else {
      std::string path;
      get_string(fileBrowser->Filename(GetCurSel()), path);

      // ChangeDisc(path);
      bHandled = FALSE;
    }
    return S_OK;
  }

public:
  // Define the class. The class name must match the ClassOverride property
  // set for the scene in the UI Authoring tool.
  XUI_IMPLEMENT_CLASS(CDiscSwapBrowser, L"DiscSwapBrowser", XUI_CLASS_LIST)
};

class CEffectBrowser : public CXuiFileBrowser {
protected:
  CEffectBrowser() { fileBrowser = &effectList; }

  virtual HRESULT OnNotifyPress(HXUIOBJ hObjPressed, BOOL &bHandled) {

    // Change the current dir
    if (fileBrowser->IsDir(GetCurSel())) {
      // Delete old item count
      DeleteItems(0, fileBrowser->Size());

      // Update filelist
      fileBrowser->UpdateDirList(fileBrowser->Filename(GetCurSel()));

      // Insert item count
      InsertItems(0, fileBrowser->Size());

      // Move to top
      SetCurSelVisible(0);

      // Don't Notify parent
      bHandled = TRUE;

    } else {
      // std::string path;
      // get_string(fileBrowser->Filename(GetCurSel()), path);
      // strcpy(xboxConfig.Shaders, path.c_str());
      // CoverSystemAndGameProfile(xboxConfig.CurrentgameID);

      // LoadShaderFromFile(path.c_str());
      // ApplyShader(path.c_str());

      bHandled = FALSE;
    }
    return S_OK;
  }

public:
  // Define the class. The class name must match the ClassOverride property
  // set for the scene in the UI Authoring tool.
  XUI_IMPLEMENT_CLASS(CEffectBrowser, L"EffectBrowser", XUI_CLASS_LIST)
};