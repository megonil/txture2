#ifndef TXTURE2_TXR_OPCODE_H
#define TXTURE2_TXR_OPCODE_H

#include <stdint.h>

#define BINARY_INSTRUCTIONS                                               \
	B (Add, '+', "add")                                                   \
	B (Sub, '-', "sub")                                                   \
	B (Mul, '*', "mul")                                                   \
	B (Div, '/', "div")                                                   \
	B (Pow, '^', "pow")

#define B(Variant, Ch, Str) Variant,

typedef enum : uint8_t { Expand, Const, BINARY_INSTRUCTIONS } opcode;

#undef B

#endif // !TXTURE2_TXR_OPCODE_H
