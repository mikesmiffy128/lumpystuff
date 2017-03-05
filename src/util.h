#pragma once

#define CAT1(a, b) a##b
#define CAT(a, b) CAT1(a, b)

#define L(str) L"" CAT1(L, str)

static __forceinline size_t size_min(size_t a, size_t b) {
	if (b < a) {
		return b;
	}
	return a;
}

static __forceinline size_t size_max(size_t a, size_t b) {
	if (b > a) {
		return b;
	}
	return a;
}
