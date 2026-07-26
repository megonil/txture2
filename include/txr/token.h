#ifndef TXTURE2_TXR_TOKEN_H
#define TXTURE2_TXR_TOKEN_H

#include <limits.h>

#define KEYWORDS K (Alias, "alias")
#define K(Variant, Str) TT##Variant,

typedef enum {
	TTStart = UCHAR_MAX + 1,
	TTError,
	TTNumber,
	TTId,
	TTEof,
	KEYWORDS
} TokenType;

#undef K
#endif // !TXTURE2_TXR_TOKEN_H
