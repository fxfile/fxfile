#ifndef __FXB_VWIN32_H__
#define __FXB_VWIN32_H__
#pragma once

#ifndef VWIN32_DIOC_DOS_IOCTL
#define VWIN32_DIOC_DOS_IOCTL 1

typedef struct tag_DIOC_REGISTERS {
	DWORD reg_EBX;
	DWORD reg_EDX;
	DWORD reg_ECX;
	DWORD reg_EAX;
	DWORD reg_EDI;
	DWORD reg_ESI;
	DWORD reg_Flags;
} DIOC_REGISTERS, *PDIOC_REGISTERS;

#endif

// Intel x86 processor status fla
#define CARRY_FLAG  0x1

#pragma pack(push, 1)

typedef struct tag_DOSDPB {
	BYTE    specialFunc;    //
	BYTE    devType;        //
	WORD    devAttr;        //
	WORD    cCyl;           // number of cylinders
	BYTE    mediaType;      //
	WORD    cbSec;          // Bytes per sector
	BYTE    secPerClus;     // Sectors per cluster
	WORD    cSecRes;        // Reserved sectors
	BYTE    cFAT;           // FATs
	WORD    cDir;           // Root Directory Entries
	WORD    cSec;           // Total number of sectors in image
	BYTE    bMedia;         // Media descriptor
	WORD    secPerFAT;      // Sectors per FAT
	WORD    secPerTrack;    // Sectors per track
	WORD    cHead;          // Heads
	DWORD   cSecHidden;     // Hidden sectors
	DWORD   cTotalSectors;  // Total sectors, if cbSec is zero
	BYTE    reserved[6];    //
} DOSDPB, *PDOSDPB;

#pragma pack(pop)

#endif // __FXB_VWIN32_H__
