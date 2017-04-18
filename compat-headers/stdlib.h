#pragma once

#ifdef _WIN32
	#include <../ucrt/stdlib.h>
	// You have got to be kidding me.
	#undef min
	#undef max
#else
	#include_next <stdlib.h>
#endif
