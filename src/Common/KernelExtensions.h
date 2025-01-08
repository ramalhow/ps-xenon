#pragma once
#include <winbase.h>

typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
    PCHAR Buffer;
} STRING, *PSTRING;

extern "C"
{
    HRESULT WINAPI ObCreateSymbolicLink(IN PSTRING SymbolicLinkName, IN PSTRING DeviceName);
    HRESULT WINAPI ObDeleteSymbolicLink(IN PSTRING SymbolicLinkName);
}