#include "txr/vm.h"

#include "array.h"
#include "txr/chunk.h"
#include "txr/opcode.h"
#include "utils.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

VM*
make_vm ()
{
	VM* vm	  = malloc (sizeof (VM));
	vm->stack = array (tvalue);

	return vm;
}

inline void
vm_free (VM* vm)
{ free (vm); }

#define index()                                                           \
	uint32_t index;                                                       \
	if (expanded) {                                                       \
		uint8_t high = *code++;                                           \
		uint8_t mid	 = *code++;                                           \
		uint8_t low	 = *code++;                                           \
		index		 = from_u24 ();                                       \
	} else {                                                              \
		index = *code++;                                                  \
	}

#define spush(v) push (vm->stack, (v))
#define spop() pop (vm->stack)

static inline tvalue
value_Add (tvalue a, tvalue b)
{ return a + b; }

static inline tvalue
value_Sub (tvalue a, tvalue b)
{ return a - b; }

static inline tvalue
value_Mul (tvalue a, tvalue b)
{ return a * b; }

static inline tvalue
value_Div (tvalue a, tvalue b)
{ return a / b; }

static inline tvalue
value_Pow (tvalue a, tvalue b)
{ return pow (a, b); }

#define U(Variant, Ch, Str, Op)                                           \
	case Variant: {                                                       \
		spush (Op (spop ()));                                             \
		break;                                                            \
	}

#define B(Variant, Ch, Str, Prec)                                         \
	case Variant: {                                                       \
		tvalue b = spop ();                                               \
		tvalue a = spop ();                                               \
		spush (value_##Variant (a, b));                                   \
		break;                                                            \
	}

void
execute (VM* vm, Chunk* chunk)
{
	opcode* code	 = chunk->code;
	uint8_t expanded = 0;
	for (;;) {
		if (code > chunk->code + len (chunk->code)) break;
		// clang-format off
		switch (*code++) {
		case Expand: expanded = 1; continue;
		case Const: {
			index ();
			assert (index < len (chunk->constants));
			spush (chunk->constants[index]);
			break;
		}

		BINARY_INSTRUCTIONS
		UNARY_INSTRUCTIONS
		}
		// clang-format on

		expanded = 0;
	}
}

#undef B
#undef U
