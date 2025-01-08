#pragma once

// lightweight master include for xbox aplications
// inspired by https://github.com/Leandros/WindowsHModular

// define the platform macros
#include <xdk.h>
#define WINVER 0x0500
#define _XBOX_VER 200
#define _PPC_

// Disabled warnings
#pragma warning(disable : 4001) // 'single line comment' was used (kinda ironic)
#pragma warning(disable : 4100) // unreferenced formal parameter
#pragma warning(disable : 4103) // alignment changed after including header (missing #pragma pack(pop) ?)
#pragma warning(disable : 4201) // nameless struct/union
#pragma warning(disable : 4214) // bit field types other than int
#pragma warning(disable : 4514) // unreferenced inline function has been removed

// experimental
#define NOMINMAX

// System Headers
#include <stdlib.h>
#include <stdarg.h>
#include <windef.h>
#include <winbase.h>
#include <xbox.h>
#include <xam.h>

#ifdef ENABLE_PIX
#include <pix.h>
#endif // ENABLE_PIX

#ifdef INCLUDE_D3D
#define D3DCOMPILE_USEVOIDS // Change the return type of DiretcX functions to void
#include <d3d9.h>
#include <d3dx9.h>
#endif // INCLUDE_D3D

#ifdef INCLUDE_NET
#include <winsockx.h>
#include <xonline.h>
#endif // INCLUDE_NET