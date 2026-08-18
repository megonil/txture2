#ifndef TXTURE2_TXR_OPCODE_H
#define TXTURE2_TXR_OPCODE_H

#include <stdint.h>

#define BINARY_INSTRUCTIONS                                               \
	B (Add, '+', "add", PTerm)                                            \
	B (Sub, '-', "sub", PTerm)                                            \
	B (Mul, '*', "mul", PFactor)                                          \
	B (Div, '/', "div", PFactor)                                          \
	B (Mod, '%', "mod", PFactor)                                          \
	B (Pow, '^', "pow", PPow)                                             \
	B (BinAnd, '&', "binand", PBinAnd)                                    \
	B (BinOr, '|', "binor", PBinOr)                                       \
	B (BinXor, '$', "binxor", PBinXor)                                    \
	B (Shl, TTShl, "shl", PShift)                                         \
	B (Shr, TTShr, "shr", PShift)                                         \
	B (Ge, TTGe, "ge", PCmp)                                              \
	B (Gt, '>', "gt", PCmp)                                               \
	B (Le, TTLe, "le", PCmp)                                              \
	B (Lt, '<', "lt", PCmp)                                               \
	B (Eq, TTEq, "eq", PCmp)                                              \
	B (Neq, TTNEq, "neq", PEq)                                            \
	B (And, TTAnd, "and", PAnd)                                           \
	B (Or, TTOr, "or", POr)

#define UNARY_INSTRUCTIONS                                                \
	U (Neg, '-', "neg", -)                                                \
	U (Not, '!', "not", !)

#define B(Variant, Ch, Str, Prec) Variant,
#define U(Variant, Ch, Str, Op) Variant,

typedef enum : uint8_t {
	Expand,
	Const,
	Pop,
	Set,  // <expression>, const <variable name index> (<variable name> Set
	Load, // const <variable name index> (<variable name>), Load
	Call,
	BINARY_INSTRUCTIONS UNARY_INSTRUCTIONS
} opcode;

#undef U
#undef B

#endif // !TXTURE2_TXR_OPCODE_H
