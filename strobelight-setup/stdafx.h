/* Defines ******************************************************************/

#define _CRT_SECURE_NO_WARNINGS

/* Includes *****************************************************************/

#include <windows.h>
#include <tchar.h>
#include <setupapi.h>

#include "resource.h"

/* Libraries ****************************************************************/

#pragma comment(lib, "setupapi.lib")

/* Constants ****************************************************************/

const GUID GUID_DEVINTERFACE_MONITOR = {0xE6F07B5F, 0xEE97, 0x4A90, {0xB0, 0x76, 0x33, 0xF5, 0x7B, 0xF4, 0xEA, 0xA7}};

/****************************************************************************/
