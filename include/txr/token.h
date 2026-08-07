#ifndef TXTURE2_TXR_TOKEN_H
#define TXTURE2_TXR_TOKEN_H

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>

#define MULTIPLECHARS                                                     \
	M ('=', "=", Eq, '=', "=")                                            \
	M ('!', "!", NEq, '=', "=")

#define AMBIGUOUS_MULTIPLECHARS                                           \
	A ('>', ">", '=', "=", '>', ">", Ge, Shr)                             \
	A ('<', "<", '=', "=", '<', "<", Le, Shl)

#define PRINTABLE_TOKENS                                                  \
	P (TTId, "identificator")                                             \
	P (TTNumber, "number")

#define KEYWORDS                                                          \
	K (Alias, "alias")                                                    \
	K (Fn, "fn")                                                          \
	K (End, "end")                                                        \
	K (And, "and")                                                        \
	K (Or, "or")

#define str_from2(c, c2)                                                  \
	(const char[])                                                        \
	{ c, c2 }

#define TOKENS                                                            \
	KEYWORDS PRINTABLE_TOKENS MULTIPLECHARS AMBIGUOUS_MULTIPLECHARS

#define K(Variant, Str) TT##Variant,
#define M(Ch, Chs, Variant, Ch2, Ch2s) TT##Variant,
#define A(Ch, Chs, Left, Ls, Right, Rs, V1, V2) TT##V1, TT##V2,
#define P(Variant, Str) Variant,

typedef enum {
	TTStart = UCHAR_MAX + 1,
	TTError,
	TOKENS TTEof,
	TTEndTokens,
} TokenType;

#undef P
#undef K
#undef A
#undef M

#define K(Variant, Str) Str,
#define P(Variant, Str) Str,
#define M(Ch, Chs, Variant, Ch2, Ch2s) Chs Ch2s,

#define A(Ch, Chs, Left, Ls, Right, Rs, V1, V2) Chs Ls, Chs Rs,

// starts from TTError
static const char* const toks_strs[] = {"<error>", TOKENS "<EOF>"};

#undef K
#undef P
#undef M
#undef A

const char*
tok_to_string (TokenType t);

#endif // !TXTURE2_TXR_TOKEN_H
