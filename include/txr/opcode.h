#ifndef TXTURE2_TXR_OPCODE_H
#define TXTURE2_TXR_OPCODE_H

#include <stdint.h>

#define BINARY_INSTRUCTIONS                                               \
	B (Add, '+', "add", PTerm)                                            \
	B (Sub, '-', "sub", PTerm)                                            \
	B (Mul, '*', "mul", PFactor)                                          \
	B (Div, '/', "div", PFactor)                                          \
	B (Pow, '^', "pow", PPow)

#define UNARY_INSTRUCTIONS                                                \
	U (Neg, '-', "neg", -)                                                \
	U (Not, '!', "not", !)

#define B(Variant, Ch, Str, Prec) Variant,
#define U(Variant, Ch, Str, Op) Variant,

typedef enum : uint8_t {
	Expand,
	Const,
	BINARY_INSTRUCTIONS UNARY_INSTRUCTIONS
} opcode;

#undef U
#undef B

#endif // !TXTURE2_TXR_OPCODE_H
