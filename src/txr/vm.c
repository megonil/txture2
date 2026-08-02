#include "txr/vm.h"

#include "array.h"
#include "table.h"
#include "txr/chunk.h"
#include "txr/opcode.h"
#include "utils.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define none 0
#define nonearr                                                           \
	(const char*[])                                                       \
	{ none, none, none, none }

// this one will get it's use in the future
size_t _Thread_local line = 1;

#define default_last 0.0

#define result(eline, ekind, aargs, llast)                                \
	(VMResult)                                                            \
	{                                                                     \
		.code = {.line = eline, .kind = ekind},                           \
		.args = {aargs[0], aargs[1], aargs[2], aargs[3]}, .last = llast   \
	}

#define ok(v) return result (line, VMOK, nonearr, v)

#define args1(a) ((const char*[]){a, NULL, NULL, NULL})
#define args2(a, b) ((const char*[]){a, b, NULL, NULL})
#define args3(a, b, c) ((const char*[]){a, b, c, NULL})
#define args4(a, b, c, d) ((const char*[]){a, b, c, d})

#define err(kind, args) return result (line, kind, args, default_last)

VM*
make_vm ()
{
	VM* vm	  = malloc (sizeof (VM));
	vm->stack = array (tvalue);
	VariableTableInit (&vm->variables);

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

VMResult
execute (VM* vm, const Chunk* chunk)
{
	opcode* code	 = chunk->code;
	uint8_t expanded = 0;
	for (;;) {
		if (code > chunk->code + len (chunk->code)) break;
		// clang-format off
		switch (*code++) {
			case Expand: expanded = 1; continue;
			case Pop: (void)spop(); break;
			case Set: {
				tvalue variable_value = spop();

				index();
				const char* variable_name = chunk->strings_to_free[index];

				VariableTableInsert(&vm->variables, variable_name, variable_value);
				break;
			}
			case Load: {
				index();
				const char* variable_name = chunk->strings_to_free[index];

				if (variable_name == 0 || !VariableTableContains(&vm->variables, variable_name)) {
					err(UnknownVariable, args1(variable_name));
				}

				spush(*VariableTableGet(&vm->variables, variable_name));
				break;
			}
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

	tvalue last = default_last;
#ifndef TXTURE2_DEBUG
	while (!array_empty (vm->stack)) last = spop ();
#endif

	ok (last);
}

#undef B
#undef U

#define position " at %zu"
#define V(Variant, Fmt)                                                   \
	case Variant:                                                         \
		fprintf (                                                         \
			stderr, Fmt, e.args[0], e.args[1], e.args[2], e.args[3]);     \
		break;

void
vm_print (VMResult e)
{
	if (e.code.kind == VMOK) return;
	switch (e.code.kind) {
		VMERRORS
		default: __builtin_unreachable ();
	}

	eprintln (position, e.code.line);
}

#undef V
