#ifndef TXTURE2_TXR_MACROSBUILTIN_H
#define TXTURE2_TXR_MACROSBUILTIN_H

#include "macro.h"

#include <math.h>

#define BUILTIN_MACRO_MAX_ARGS 3

#define BUILTIN_ALIASES                                                   \
	BA (PI, _value_tt (M_PI))                                             \
	BA (E, _value_tt (M_E))                                               \
	BA (INF, _value_tt (HUGE_VAL))                                        \
	BA (LOG2E, _value_tt (M_LOG2E))                                       \
	BA (LOG10E, _value_tt (M_LOG10E))                                     \
	BA (LN2, _value_tt (M_LN2))                                           \
	BA (LN10, _value_tt (M_LN10))                                         \
	BA (SQRT2, _value_tt (M_SQRT2))                                       \
	BA (INVERSE_SQRT2, _value_tt (M_SQRT1_2))

#define _macrostr_array_ (char* [BUILTIN_MACRO_MAX_ARGS])
#define _macrocode_array_ (ExtendedToken[])

#define BUILTIN_MACROS                                                    \
	BM (expand_v,                                                         \
		(_macrostr_array_{"v", 0, 0}),                                    \
		(_macrocode_array_{_id_tt ("v"), _just_tt ('*'), _id_tt ("M")}))

#endif // !TXTURE2_TXR_MACROSBUILTIN_H
