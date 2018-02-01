#pragma once

#include <stdint.h>

enum Endian {
	LITTLE_ENDIAN = 0,
	BIG_ENDIAN = 1,
	
	MACHINE_ENDIAN =
		#ifdef __BYTE_ORDER__
			#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
				LITTLE_ENDIAN
			#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
				BIG_ENDIAN
			#endif
		#elif defined(__i386__) || defined(__LP64__) || defined(_M_IX86) || \
				defined(_M_AMD64)
			LITTLE_ENDIAN
		#elif
			#error "Could not determine system endian; see endian.h."
		#endif
};

// Converts between little-endian and the machine's endian (no-op on most PCs).
static inline int32_t LESwap32(int32_t i) {
// Constant comparison
#ifdef _WIN32
	#pragma warning(push, 0)
#endif
	if (MACHINE_ENDIAN == LITTLE_ENDIAN) {
#ifdef _WIN32
	#pragma warning(pop)
#endif
		return i;
	}
	else {
		#if defined(__GNUC__) || defined(__clang__)
			return __builtin_bswap32(i);
		#else
			return ((i >> 24) & 0xff)	   | // 3 -> 0
				   ((i <<  8) & 0xff0000)  | // 1 -> 2
				   ((i >>  8) & 0xff00)	   | // 2 -> 1
				   ((i << 24) & 0xff000000); // 0 -> 3
		#endif
	}
}
