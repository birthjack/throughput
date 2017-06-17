#ifndef _THROUGHPUTTEST_H_
#define _THROUGHPUTTEST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chrapi.h"
#include "stdafx.h"

void show_error(CHR_HANDLE handle, CHR_API_RC code, CHR_STRING where);
CHR_FLOAT beginThroughputTest();
DWORD WINAPI closeShield(void);
DWORD WINAPI openShield(void);
bool waitDUTReStartup();
bool waitDUTStartup();

bool pingRequest(char IPaddr[16]);
bool pingRequestAgain(char IPaddr[16]);

#endif