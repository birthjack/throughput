/*******************************************************************************
  Copyright (C), 2013 TP-LINK TECHNOLOGIES CO., LTD.

  File name:   fmifs.h

  Description: 定义和fmifs.dll相关的载入、函数获取等变量和函数

  Author:      Li Guoliang <liguoliang@tp-link.net>

  History:
  ---------------------------------
  V1.0, 2013-12-02, liguoliang      create file.
*******************************************************************************/

#ifndef _FMIFS_H_
#define _FMIFS_H_

#include <afx.h>

typedef struct {
	DWORD Lines;
	PCHAR Output;
} TEXTOUTPUT, *PTEXTOUTPUT;

//
// Callback command types
//
typedef enum {
	PROGRESS,
	DONEWITHSTRUCTURE,
	UNKNOWN2,
	UNKNOWN3,
	UNKNOWN4,
	UNKNOWN5,
	INSUFFICIENTRIGHTS,
	UNKNOWN7,
	UNKNOWN8,
	UNKNOWN9,
	UNKNOWNA,
	DONE,
	UNKNOWNC,
	UNKNOWND,
	OUTPUT,
	STRUCTUREPROGRESS
} CALLBACKCOMMAND;

//
// FMIFS callback definition
//
typedef BOOLEAN (__stdcall *PFMIFSCALLBACK)( CALLBACKCOMMAND Command, DWORD SubAction, PVOID ActionInfo );

//
// Chkdsk command in FMIFS
//
typedef VOID (__stdcall *PCHKDSK)( PWCHAR DriveRoot, 
	PWCHAR Format,
	BOOL CorrectErrors, 
	BOOL Verbose, 
	BOOL CheckOnlyIfDirty,
	BOOL ScanDrive, 
	PVOID Unused2, 
	PVOID Unused3,
	PFMIFSCALLBACK Callback );

//
// Format command in FMIFS
//

// media flags 
#define FMIFS_REMOVEABLEDISK	0x0B   //--removeable disk

// media flags
#define FMIFS_HARDDISK 0xC
#define FMIFS_FLOPPY   0x8

typedef VOID (__stdcall *PFORMATEX)( PWCHAR DriveRoot,
	DWORD MediaFlag,
	PWCHAR Format,
	PWCHAR Label,
	BOOL QuickFormat,
	DWORD ClusterSize,
	PFMIFSCALLBACK Callback );

extern PFORMATEX FormatEx;
extern HMODULE fmisfDll;

#endif