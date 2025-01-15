#include "FileUtil.hpp"

#include <Common/CommonXbox.h>

namespace File {

    bool PathExists(const char* path) {
        return GetFileAttributes(path) != (DWORD)-1;
    }


}