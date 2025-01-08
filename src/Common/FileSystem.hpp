#pragma once

namespace FileSystem
{
    bool Init();
    void Shutdown();
    bool MountPath(char *DriveName, char *AbsoluteDrivePath);
    bool UnmountPath(char *DriveName);
    bool SetupDefaultDirectories();

    // TODO: make use of the XContent API for acessing and mounting drives on xbox
    // than we can support per user configuration
    namespace XContent
    {
        void SelectStorageDevice();
        bool MountDrive(const char *DriveName, const char *AbsoluteDrivePath);
        bool UnmountDrive(const char *DriveName);
    }
}