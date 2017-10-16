#pragma once
#include "stdafx.h"

#define EXE_NAME "MergeWinICO.exe"
#define VERSION "MergeWinICO 1.0 (16 October 2017)"
#define DEBUG false

#define LEN_HEADER sizeof(WINICO_HEADER)
#define LEN_ENTRY sizeof(WINICO_ENTRY)

struct WINICO_HEADER {
	unsigned short reserved;
	unsigned short type;
	unsigned short count;
	//$assert(reserved == 0, "Not a WINICO file");
	//$assert(type == 1, "Unsupported image type");
};

struct WINICO_ENTRY {
	unsigned char width;
	unsigned char height;
	unsigned char palette;
	unsigned char magic;

	unsigned short planes;
	unsigned short bpp;
	unsigned long size;
	unsigned long offset;
};