#pragma once

#if defined(_MSC_VER) && !defined(__clang__)
	#define _Noreturn __declspec(noreturn)
#endif
#define noreturn _Noreturn
