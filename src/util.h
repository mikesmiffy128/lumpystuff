#pragma once

#define CAT1(a, b) a##b
#define CAT(a, b) CAT1(a, b)

#define L(str) L"" CAT1(L, str)
