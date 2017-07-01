#pragma once

#include <stdint.h>

// This won't fit in an enum because it's not an arithmetic constexpr - I think.
// We define VBSPHEADER like this so endian doesn't affect its value.
#define VBSPHEADER ((const union { char s[4]; int32_t i; }) {"VBSP"}.i)

enum {
	MINBSPVERSION = 19,
	BSPVERSION = 21,
	HEADER_LUMPS = 64
};

#pragma pack(push, 1)

struct BSPHeader {
	int32_t ident;
	int32_t version;
	struct {
		int32_t fileofs;
		int32_t filelen;
		int32_t version;
		char fourCC[4];
	} lumps[HEADER_LUMPS];
	int32_t mapRevision;
};

struct LMPHeader {
	int32_t lumpOffset;
	int32_t lumpID;
	int32_t lumpVersion;
	int32_t lumpLength;
	int32_t mapRevision;
};

#pragma pack(pop)
