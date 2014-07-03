#ifndef __utils__
#define __utils__

#include "system.h"

DWORD MakePixels(BYTE* obuffer, BYTE* ibuffer, long cx, long cy);
DWORD MakeFontSetText(BYTE *obuffer, char* str, char* fontfile, DWORD fontcolor);

#endif
