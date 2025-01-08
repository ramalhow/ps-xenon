#include "FileSystem.hpp"

#include <Common/Types.h>
#include <Common/CommonXbox.h>
#include <Common/KernelExtensions.h>
#include <string.h>

static HANDLE MountStaticMappedDrive(const char *DrivePath) {
	return CreateFile(
		DrivePath,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
		nullptr);
}

namespace FileSystem
{
	bool MountPath(char *DriveName, char *AbsoluteDrivePath)
    {

        // very ugly... but this is what the xboxkrnl.lib exports to us
        // according to the original source
        STRING Drive = {
            (uint16) strnlen_s(DriveName, 4), // the symbol length is always 4
            5,
            DriveName
        };

        STRING AbsolutePath = {
            (uint16) strnlen_s(AbsoluteDrivePath, MAX_PATH),
            MAX_PATH + 1,
            AbsoluteDrivePath
        };

        return ObCreateSymbolicLink(&Drive, &AbsolutePath) == S_OK;
    };

    bool UnmountPath(char *DriveName)
    {
        STRING Drive = {
            (uint16) strnlen_s(DriveName, 4),
            5,
            DriveName
        };
        return ObDeleteSymbolicLink(&Drive) == S_OK;
    }

    bool Init()
    {
        OutputDebugString("Init Filesystem");
		MountPath("hdd0:", "\\Device\\Harddisk0\\Partition0");
        MountPath("hdd1:", "\\Device\\Harddisk0\\Partition1");
        MountPath("hdd2:", "\\Device\\Harddisk0\\Partition2");
        MountPath("hdd3:", "\\Device\\Harddisk0\\Partition3");

        MountPath("usb0:", "\\Device\\Mass0");
        MountPath("usb1:", "\\Device\\Mass1");
        MountPath("usb2:", "\\Device\\Mass2");

        //MountPath("dvd:", "\\Device\\DVD");
        return true;
    };

	void Shutdown()
    {
        //UnmountPath("dvd:");

        UnmountPath("usb2:");
        UnmountPath("usb1:");
        UnmountPath("usb0:");
        
        UnmountPath("hdd3:");
        UnmountPath("hdd2:");
        UnmountPath("hdd1:");
        UnmountPath("hdd0:");        
    }

    bool SetupDefaultDirectories()
    {

        return true;
    }
    
}