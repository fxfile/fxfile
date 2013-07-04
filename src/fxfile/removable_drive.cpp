//
// Copyright (c) 2001-2012 Leon Lee author. All rights reserved.
//
//   homepage: http://www.flychk.com
//   e-mail:   mailto:flychk@flychk.com
//
// Use of this source code is governed by a GPLv3 license that can be
// found in the LICENSE file.

#include "stdafx.h"
#include "removable_drive.h"

#include <setupapi.h>
#include <dbt.h>                        // for DeviceChange
#include <winioctl.h>                   // for DeviceIOCtl
#include <cfgmgr32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace fxfile
{
#if (_MSC_VER < 1500)

// retrieve the storage device descriptor data for a device. 
typedef struct _STORAGE_DEVICE_DESCRIPTOR
{
    ULONG  Version;
    ULONG  Size;
    UCHAR  DeviceType;
    UCHAR  DeviceTypeModifier;
    BOOLEAN  RemovableMedia;
    BOOLEAN  CommandQueueing;
    ULONG  VendorIdOffset;
    ULONG  ProductIdOffset;
    ULONG  ProductRevisionOffset;
    ULONG  SerialNumberOffset;
    STORAGE_BUS_TYPE  BusType;
    ULONG  RawPropertiesLength;
    UCHAR  RawDeviceProperties[MAX_PATH];
} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;

// retrieve the properties of a storage device or adapter. 
typedef enum _STORAGE_QUERY_TYPE {
    PropertyStandardQuery = 0,
    PropertyExistsQuery,
    PropertyMaskQuery,
    PropertyQueryMaxDefined
} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;

// retrieve the properties of a storage device or adapter. 
typedef enum _STORAGE_PROPERTY_ID {
    StorageDeviceProperty = 0,
    StorageAdapterProperty,
    StorageDeviceIdProperty
} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;

// retrieve the properties of a storage device or adapter. 
typedef struct _STORAGE_PROPERTY_QUERY {
    STORAGE_PROPERTY_ID  PropertyId;
    STORAGE_QUERY_TYPE  QueryType;
    UCHAR  AdditionalParameters[1];
} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;

#endif

// IOCTL control code
#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

static xpr_bool_t getDisksProperty(HANDLE aDeviceFile, PSTORAGE_DEVICE_DESCRIPTOR aStorageDeviceDescriptor)
{
    STORAGE_PROPERTY_QUERY sStoragePropertyQuery; // input param for query
    DWORD sOutBytes; // IOCTL output length
    xpr_bool_t sResult; // IOCTL return val

    // specify the query type
    sStoragePropertyQuery.PropertyId = StorageDeviceProperty;
    sStoragePropertyQuery.QueryType = PropertyStandardQuery;

    // sStoragePropertyQuery using IOCTL_STORAGE_QUERY_PROPERTY 
    sResult = ::DeviceIoControl(
        aDeviceFile, // device handle
        IOCTL_STORAGE_QUERY_PROPERTY, // info of device property
        &sStoragePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY), // input data buffer
        aStorageDeviceDescriptor, aStorageDeviceDescriptor->Size, // output data buffer
        &sOutBytes, // out's length
        (LPOVERLAPPED)XPR_NULL); 

    return sResult;
}

RemovableDrive::RemovableDrive(void)
{
}

RemovableDrive::~RemovableDrive(void)
{
}

xpr_bool_t RemovableDrive::isRemovableDrive(xpr_tchar_t aDriveChar)
{
    xpr_tchar_t sDevicePath[0xff] = {0};
    _stprintf(sDevicePath, XPR_STRING_LITERAL("\\\\?\\%c:"), aDriveChar);

    HANDLE sDeviceFile = ::CreateFile(sDevicePath, 0, FILE_SHARE_READ, XPR_NULL, OPEN_EXISTING, XPR_NULL, XPR_NULL);
    if (sDeviceFile == INVALID_HANDLE_VALUE)
        return XPR_FALSE;

    xpr_char_t *sStorageDeviceDescriptorBuffer = new xpr_char_t[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 1024];
    if (XPR_IS_NULL(sStorageDeviceDescriptorBuffer))
    {
        ::CloseHandle(sDeviceFile);
        return XPR_FALSE;
    }

    xpr_bool_t sUsbDrive = XPR_FALSE;

    PSTORAGE_DEVICE_DESCRIPTOR sStorageDeviceDescriptor = (PSTORAGE_DEVICE_DESCRIPTOR)sStorageDeviceDescriptorBuffer;
    sStorageDeviceDescriptor->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 1024;

    if (getDisksProperty(sDeviceFile, sStorageDeviceDescriptor) == XPR_TRUE)
    {
        if (sStorageDeviceDescriptor->BusType == BusTypeUsb)
        {
            sUsbDrive = XPR_TRUE;
        }
    }

    XPR_SAFE_DELETE_ARRAY(sStorageDeviceDescriptorBuffer);

    ::CloseHandle(sDeviceFile);

    return sUsbDrive;
}

void RemovableDrive::getRemovableDriveList(DriveDeque &aDriveDeque)
{
    aDriveDeque.clear();

    xpr_tchar_t sDriveStrings[0xff] = {0};
    GetLogicalDriveStrings(0xfe, sDriveStrings);

    xpr_tchar_t sDriveChar;
    xpr_tchar_t sDrivePath[XPR_MAX_PATH + 1] = {0};
    xpr_uint_t sDriveType;

    xpr_tchar_t *sDriveEnum = sDriveStrings;

    while (sDriveEnum[0] != '\0')
    {
        sDriveChar = sDriveEnum[0];
        sDriveChar &= ~0x20; // uppercase

        if (sDriveChar != 'A' && sDriveChar != 'B') // except for floppy disk (FDD)
        {
            _stprintf(sDrivePath, XPR_STRING_LITERAL("%c:\\"), sDriveChar);
            sDriveType = GetDriveType(sDrivePath);

            if (sDriveType == DRIVE_FIXED || sDriveType == DRIVE_REMOVABLE)
            {
                if (isRemovableDrive(sDriveChar))
                {
                    aDriveDeque.push_back(sDriveChar);
                }
            }
        }

        sDriveEnum += _tcslen(sDriveEnum) + 1;
    }
}

//
// @ usb eject function
//
static DEVINST getDrivesDevInstByDiskNumber(xpr_slong_t aDiskNumber, xpr_uint_t aDriveType, xpr_tchar_t *aDosDeviceName)
{
    xpr_bool_t sFloppy = (_tcsstr(aDosDeviceName, XPR_STRING_LITERAL("\\Floppy")) != XPR_NULL);

    GUID *sGuid;

    switch (aDriveType) 
    {
    case DRIVE_REMOVABLE:
        {
            sGuid = XPR_IS_TRUE(sFloppy) ? (GUID*)(void*)&GUID_DEVINTERFACE_FLOPPY : (GUID*)(void*)&GUID_DEVINTERFACE_DISK;
            break;
        }
    case DRIVE_FIXED:
        {
            sGuid = (GUID*)(void*)&GUID_DEVINTERFACE_DISK;
            break;
        }
    case DRIVE_CDROM:
        {
            sGuid = (GUID*)(void*)&GUID_DEVINTERFACE_CDROM;
            break;
        }
    default:
        return 0;
    }

    // Get device interface info set handle for all devices attached to system
    HDEVINFO sDevInfo = ::SetupDiGetClassDevs(sGuid, XPR_NULL, XPR_NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (sDevInfo == INVALID_HANDLE_VALUE)
        return 0;

    // Retrieve a context structure for a device interface of a device
    // information set.
    DWORD sIndex = 0;
    SP_DEVICE_INTERFACE_DATA sTempSpDeviceInterfaceData = {0};
    sTempSpDeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    xpr_bool_t sResult = XPR_FALSE;

    xpr_byte_t sBuffer[1024];
    PSP_DEVICE_INTERFACE_DETAIL_DATA sSpDeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)sBuffer;
    SP_DEVICE_INTERFACE_DATA sSpDeviceInterfaceData;
    SP_DEVINFO_DATA sSpDevInfoData;
    DWORD sSize;

    sSpDeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    while (true)
    {
        sResult = ::SetupDiEnumDeviceInterfaces(sDevInfo, XPR_NULL, sGuid, sIndex, &sTempSpDeviceInterfaceData);
        if (XPR_IS_FALSE(sResult)) 
            break;

        ::SetupDiEnumInterfaceDevice(sDevInfo, XPR_NULL, sGuid, sIndex, &sSpDeviceInterfaceData);

        sSize = 0;
        ::SetupDiGetDeviceInterfaceDetail(sDevInfo, &sSpDeviceInterfaceData, XPR_NULL, 0, &sSize, XPR_NULL);

        if (sSize != 0 && sSize <= sizeof(sBuffer))
        {
            sSpDeviceInterfaceDetailData->cbSize = sizeof(*sSpDeviceInterfaceDetailData); // 5 Bytes!

            memset(&sSpDevInfoData, 0, sizeof(sSpDevInfoData));
            sSpDevInfoData.cbSize = sizeof(sSpDevInfoData);

            xpr_bool_t sResult = ::SetupDiGetDeviceInterfaceDetail(sDevInfo, &sSpDeviceInterfaceData, sSpDeviceInterfaceDetailData, sSize, &sSize, &sSpDevInfoData);
            if (XPR_IS_TRUE(sResult))
            {

                // the device instance id string contains the serial number if the 
                // device has one, otherwise a generated id that contains the '&' char
                // xpr_char_t szDevInstId[260] = {0};
                // SetupDiGetDeviceInstanceId(sDevInfo, &sSpDevInfoData, szDevInstId, 260, XPR_NULL);
                // printf("DevInstId=%s\n", szDevInstId);

                HANDLE sDriveFile = ::CreateFile(sSpDeviceInterfaceDetailData->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, XPR_NULL, OPEN_EXISTING, XPR_NULL, XPR_NULL);
                if (sDriveFile != INVALID_HANDLE_VALUE)
                {
                    STORAGE_DEVICE_NUMBER sStorageDeviceNumber;
                    DWORD sBytesReturned = 0;
                    sResult = DeviceIoControl(sDriveFile, IOCTL_STORAGE_GET_DEVICE_NUMBER, XPR_NULL, 0, &sStorageDeviceNumber, sizeof(sStorageDeviceNumber), &sBytesReturned, XPR_NULL);
                    if (XPR_IS_TRUE(sResult))
                    {
                        if (aDiskNumber == (xpr_slong_t)sStorageDeviceNumber.DeviceNumber)
                        {
                            ::CloseHandle(sDriveFile);
                            ::SetupDiDestroyDeviceInfoList(sDevInfo);
                            return sSpDevInfoData.DevInst;
                        }
                    }
                    ::CloseHandle(sDriveFile);
                }
            }
        }
        sIndex++;
    }

    ::SetupDiDestroyDeviceInfoList(sDevInfo);

    return 0;
}

RemovableDrive::Result RemovableDrive::removeSafetyRemovableDrive(xpr_tchar_t aDriveChar)
{
    aDriveChar &= ~0x20; // uppercase

    if (aDriveChar < 'A' || aDriveChar > 'Z')
        return ResultInvalidParameter;

    xpr_tchar_t sRootPath[] = XPR_STRING_LITERAL("X:\\");   // "X:\"  -> for GetDriveType
    sRootPath[0] = aDriveChar;

    xpr_tchar_t sDevicePath[] = XPR_STRING_LITERAL("X:");   // "X:"   -> for QueryDosDevice
    sDevicePath[0] = aDriveChar;

    xpr_tchar_t sVolumeAccessPath[] = XPR_STRING_LITERAL("\\\\.\\X:");   // "\\.\X:"  -> to open the volume
    sVolumeAccessPath[4] = aDriveChar;

    xpr_slong_t sDiskNumber = -1;

    HANDLE sVolumeFile = ::CreateFile(sVolumeAccessPath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, XPR_NULL, OPEN_EXISTING, XPR_NULL, XPR_NULL);
    if (sVolumeFile == INVALID_HANDLE_VALUE)
        return ResultUnknownError;

    STORAGE_DEVICE_NUMBER sStorageDeviceNumber;
    DWORD sBytesReturned = 0;
    xpr_bool_t sResult = ::DeviceIoControl(
        sVolumeFile,
        IOCTL_STORAGE_GET_DEVICE_NUMBER,
        XPR_NULL,
        0,
        &sStorageDeviceNumber,
        sizeof(sStorageDeviceNumber),
        &sBytesReturned,
        XPR_NULL);

    if (XPR_IS_TRUE(sResult))
        sDiskNumber = sStorageDeviceNumber.DeviceNumber;

    ::CloseHandle(sVolumeFile);

    if (sDiskNumber == -1 || XPR_IS_FALSE(sResult))
        return ResultUnknownError;

    xpr_uint_t sDriveType = ::GetDriveType(sRootPath);

    xpr_tchar_t aDosDeviceName[XPR_MAX_PATH + 1];
    ::QueryDosDevice(sDevicePath, aDosDeviceName, XPR_MAX_PATH); // never fails ;-)

    DEVINST sDevInst = getDrivesDevInstByDiskNumber(sDiskNumber, sDriveType, aDosDeviceName);
    if (sDevInst == 0)
        return ResultUnknownError;

    xpr_ulong_t sStatus = 0;
    xpr_ulong_t sProblemNumber = 0;
    PNP_VETO_TYPE sVetoType = PNP_VetoTypeUnknown;
    xpr_wchar_t sVetoNameW[XPR_MAX_PATH + 1];

    DEVINST DevInstParent = 0;

    sResult = ::CM_Get_Parent(&DevInstParent, sDevInst, 0);  // disk's parent, e.g. the USB bridge, the SATA port....
    sResult = ::CM_Get_DevNode_Status(&sStatus, &sProblemNumber, DevInstParent, 0);
    xpr_bool_t sParentRemovable = ((sStatus & DN_REMOVABLE) != 0);

    xpr_bool_t sSuccess = XPR_FALSE;
    CONFIGRET sConfigRet = CR_FAILURE;

    xpr_sint_t i;
    for (i = 1; i <= 3; ++i)
    { 
        sVetoNameW[0] = 0;
        if (XPR_IS_TRUE(sParentRemovable))
        {
            sConfigRet = ::CM_Request_Device_EjectW(DevInstParent, &sVetoType, sVetoNameW, sizeof(sVetoNameW), 0);
        }
        else 
        {
            sConfigRet = ::CM_Query_And_Remove_SubTreeW(sDevInst, &sVetoType, sVetoNameW, sizeof(sVetoNameW), 0); // CM_Query_And_Remove_SubTreeA is not implemented under W2K!
        }

        sSuccess = (sConfigRet == CR_SUCCESS && sVetoType == PNP_VetoTypeUnknown);
        if (XPR_IS_TRUE(sSuccess))
            break;

        Sleep(500); // required to give the next tries a chance!
    }

    if (XPR_IS_TRUE(sSuccess))
        return ResultSucceeded;

    if (sConfigRet == CR_REMOVE_VETOED)
        return ResultRemoveVetoed;

    return ResultUnknownError;
}
} // namespace fxfile
