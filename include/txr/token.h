#ifndef TXTURE2_TXR_TOKEN_H
#define TXTURE2_TXR_TOKEN_H

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>

#define PRINTABLE_TOKENS                                                  \
	P (TTId, "identificator")                                             \
	P (TTNumber, "number")

#define KEYWORDS                                                          \
	K (Alias, "alias")                                                    \
	K (Fn, "fn")                                                          \
	K (End, "end")

#define K(Variant, Str) TT##Variant,
#define P(Variant, Str) Variant,

typedef enum {
	TTStart = UCHAR_MAX + 1,
	TTError,
	KEYWORDS PRINTABLE_TOKENS TTEof,
	TTEndTokens,
} TokenType;

#undef P
#undef K

#define K(Variant, Str) Str,
#define P(Variant, Str) Str,

// starts from TTError
static const char* const toks_strs[]
	= {"<error>", KEYWORDS PRINTABLE_TOKENS "<EOF>"};
#undef K
#undef P

const char*
tok_to_string (TokenType t);

#undef P
#undef K
#endif // !TXTURE2_TXR_TOKEN_H
