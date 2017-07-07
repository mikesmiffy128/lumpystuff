#pragma once

#if defined(_WIN32) && !defined(__GNUC__) && !defined(__clang__)
	#include <../ucrt/stdlib.h>
	// You have got to be kidding me.
	#undef min
	#undef max
#else
	#include_next <stdlib.h>
#endif
