#ifndef TXTURE2_TXR_PRAGMAS_H
#define TXTURE2_TXR_PRAGMAS_H

#include "utils.h"

#define PRAGMAS                                                           \
	P (ExpandR, "expand_r")                                               \
	P (ExpandG, "expand_g")                                               \
	P (ExpandB, "expand_b")                                               \
	P (ExpandAll, "expand")                                               \
	P (Mono, "mono")

#define P(Variant, Str) Pr##Variant,

typedef enum { PRAGMAS PrEnd = sizeof (uint) * 8 } PragmaKind;

#undef P

extern uint pragmas;

#define define_pragma(P) pragmas |= (1 << (P))
#define check_pragma(P) (pragmas >> Pr##P) & 1

#endif // !TXTURE2_TXR_PRAGMAS_H
