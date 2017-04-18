#pragma once

#ifdef _WIN32
	#include <io.h>
	#include <fcntl.h>

	#define isatty _isatty
	#define fileno _fileno
	#define setmode _setmode
#else
	#include_next <unistd.h>
#endif
