#pragma once

#ifdef _MSC_VER
	#include <../ucrt/stdlib.h>
#else
	#include_next <stdlib.h>
#endif
#ifdef _WIN32
	// You have got to be kidding me.
	#undef min
	#undef max
#endif
