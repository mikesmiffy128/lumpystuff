#pragma once

#include <stdio.h>
#include <wchar.h>

#include "util.h"

// Makeshift Unicode string stuff, somewhat like tchar but with an arbitrary-ish
// u instead of t. Mainly meant for cross-platform purposes since Windows just
// has to use UTF-16.

#ifdef _WIN32
	#define U(str) L(str)
	#define UFUNC(name) CAT1(w, name)

	#define uputs _wputs
	#define fuputs fputws
	#define uprintf wprintf
	#define fuprintf fwprintf
	#define ustrcmp wcscmp
	#define ustrtol wcstol
	#define ufopen _wfopen

	typedef wchar_t uchar;
#else
	#define U(str) str
	#define UFUNC(name) name

	#define uputs puts
	#define fuputs fputs
	#define uprintf printf
	#define fuprintf fprintf
	#define ustrcmp strcmp
	#define ustrtol strtol
	#define ufopen fopen

	typedef char uchar;
#endif
