#include <Common/CommonXbox.h>
#include <Common/FileSystem.hpp>

void __cdecl main() {

    FileSystem::Init();
    




    // Goodbye!
    FileSystem::Shutdown();
    XLaunchNewImage(XLAUNCH_KEYWORD_DASH, 0);
}