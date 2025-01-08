#pragma once


// base type
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned __int64 uint64;

typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed __int64 int64;

typedef wchar_t wchar;

// modfiers
#define RESTRICT __restrict
#define PASS_IN_REGS __declspec(passinreg)
#define FORCE_INLINE __forceinline