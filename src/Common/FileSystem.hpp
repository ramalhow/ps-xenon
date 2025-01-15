#pragma once

namespace FileSystem {
bool Init();
void Shutdown();

// Devices
bool MountPath(char *DriveName, char *AbsoluteDrivePath);
bool UnmountPath(char *DriveName);

// TODO: make use of the XContent API for acessing and mounting drives on xbox
// than we can support per user configuration
namespace XContent {
void SelectStorageDevice();
bool MountDrive(const char *DriveName, const char *AbsoluteDrivePath);
bool UnmountDrive(const char *DriveName);
} // namespace XContent
} // namespace FileSystem