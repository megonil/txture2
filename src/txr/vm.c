#include "txr/vm.h"

#include "array.h"
#include "image.h"
#include "table.h"
#include "txr/chunk.h"
#include "txr/opcode.h"
#include "txr/pragmas.h"
#include "txr/vmbuiltin.h"
#include "utils.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

extern double osc_freq;

#define none 0
#define nonearr                                                           \
	(const char*[])                                                       \
	{ none, none, none, none }

uint _Thread_local line			   = 1;
uint _Thread_local suspend_error_R = 0;
uint _Thread_local suspend_error_G = 0;
uint _Thread_local suspend_error_B = 0;

#define default_last 0.0

#define result(eline, ekind, aargs, llast)                                \
	(VMResult)                                                            \
	{                                                                     \
		.code = {.line = eline, .kind = ekind},                           \
		.args = {aargs[0], aargs[1], aargs[2], aargs[3]}, .last = llast,  \
		.color = generated_color (vm, props)                              \
	}

#define ok(v) return result (line, VMOK, nonearr, v)

#define args1(a) ((const char*[]){a, NULL, NULL, NULL})
#define args2(a, b) ((const char*[]){a, b, NULL, NULL})
#define args3(a, b, c) ((const char*[]){a, b, c, NULL})
#define args4(a, b, c, d) ((const char*[]){a, b, c, d})

#define err(kind, args) return result (line, kind, args, default_last)

#define VAR(Name, Default)                                                \
	VariableTableInsert (&vm->variables, Name, Default);
static void
builtin_variables (VM* vm){BUILTIN_VARIABLES}
#undef VAR

#define EVAR(Name, Init) VariableTableInsert (&vm->variables, Name, Init);
static void external_builtin_variables (
	VM*				  vm,
	tvalue			  x,
	tvalue			  y,
	const ImageProps* props)
{ EXTERNAL_BUILTIN_VARIABLES; }
#undef EVAR

VM*
make_vm ()
{
	VM* vm	  = malloc (sizeof (VM));
	vm->stack = array (tvalue);
	VariableTableInit (&vm->variables);

	builtin_variables (vm);

	return vm;
}

inline void
vm_free (VM* vm)
{
	VariableTableFree (&vm->variables);
	array_free (vm->stack);
	free (vm);
}

static pix
to_pix (
	tvalue			  x,
	const char*		  component,
	const ImageProps* props,
	uint*			  suspend)
{
	if (suspend) return (pix) x;

	if (x < 0) {
		warn (
			"color component '%s': %.3f < 0, clamped to 0", component, x);
		*suspend = 1;
		return 0;
	}

	if (x > props->max_colors) {
		warn (
			"color component '%s': %.3f > max value %u, clamped to %u",
			component,
			x,
			props->max_colors,
			props->max_colors);
		*suspend = 1;

		return (pix) props->max_colors;
	}

	return (pix) x;
}

#define expand(v) v * props->max_colors

#define define_component_to_pix(component)                                \
	static inline pix component##_to_pix (                                \
		tvalue component, const ImageProps* props)                        \
	{                                                                     \
		const pix result = to_pix (                                       \
			component,                                                    \
			component##_VARIABLE,                                         \
			props,                                                        \
			&suspend_error_##component);                                  \
		if (check_pragma (Expand##component)) return expand (result);     \
		return result;                                                    \
	}

define_component_to_pix (R);
define_component_to_pix (G);
define_component_to_pix (B);

static Color
generated_color (VM* vm, const ImageProps* props)
{
	tvalue R = *VariableTableGet (&vm->variables, R_VARIABLE);
	tvalue G = *VariableTableGet (&vm->variables, G_VARIABLE);
	tvalue B = *VariableTableGet (&vm->variables, B_VARIABLE);

	Color out;

	if (check_pragma (ExpandAll)) {
		R = expand (R);
		G = expand (G);
		B = expand (B);
	}

	out.r = R_to_pix (R, props);

	if (check_pragma (Mono)) {
		out.g = out.b = out.r;
		return out;
	}

	out.g = G_to_pix (G, props);
	out.b = B_to_pix (B, props);

	return out;
}

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

static inline tvalue
value_BinAnd (tvalue a, tvalue b)
{ return (tvalue) ((uint) a & (uint) b); }

static inline tvalue
value_BinOr (tvalue a, tvalue b)
{ return (tvalue) ((uint) a | (uint) b); }

static inline tvalue
value_BinXor (tvalue a, tvalue b)
{ return (tvalue) ((uint) a ^ (uint) b); }

static inline tvalue
value_Shl (tvalue a, tvalue b)
{ return (tvalue) ((uint) a << (uint) b); }

static inline tvalue
value_Shr (tvalue a, tvalue b)
{ return (tvalue) ((uint) a >> (uint) b); }

static inline tvalue
value_Ge (tvalue a, tvalue b)
{ return a >= b; }

static inline tvalue
value_Gt (tvalue a, tvalue b)
{ return a > b; }

static inline tvalue
value_Le (tvalue a, tvalue b)
{ return a <= b; }

static inline tvalue
value_Lt (tvalue a, tvalue b)
{ return a < b; }

static inline tvalue
value_Eq (tvalue a, tvalue b)
{ return a == b; }

static inline tvalue
value_Neq (tvalue a, tvalue b)
{ return a != b; }

static inline tvalue
value_Mod (tvalue a, tvalue b)
{ return fmod (a, b); }

static inline tvalue
value_And (tvalue a, tvalue b)
{ return a && b; }

static inline tvalue
value_Or (tvalue a, tvalue b)
{ return a || b; }

static inline void
sync_line (
	const Chunk*  chunk,
	const opcode* code,
	size_t*		  line_index,
	size_t*		  line_end)
{
	const size_t offset	   = (size_t) (code - chunk->code);
	const size_t lines_len = len (chunk->lines);

	while (*line_index + 1 < lines_len && offset >= *line_end) {
		++(*line_index);
		*line_end += chunk->lines[*line_index];
	}

	line = *line_index + 1;
}

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
execute (
	VM*				  vm,
	const Chunk*	  chunk,
	tvalue			  x,
	tvalue			  y,
	const ImageProps* props)
{
	len (vm->stack) = 0;
	external_builtin_variables (vm, x, y, props);

	opcode* code = chunk->code;
	opcode* end	 = chunk->code + len (chunk->code);

	size_t line_index = 0;
	size_t line_end	  = len (chunk->lines) ? chunk->lines[0] : 0;

	uint8_t expanded = 0;
	while (code < end) {
		sync_line (chunk, code, &line_index, &line_end);

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

	tvalue t = default_last;
	ok (t);
}

#undef B
#undef U

#define position "runtime error at line %u: "
#define V(Variant, Fmt)                                                   \
	case Variant:                                                         \
		eprintln (                                                        \
			position Fmt,                                                 \
			e.code.line,                                                  \
			e.args[0],                                                    \
			e.args[1],                                                    \
			e.args[2],                                                    \
			e.args[3]);                                                   \
		fflush (stderr);                                                  \
		break;

void
vm_print (VMResult e)
{
	switch (e.code.kind) {
		VMERRORS;
		default: break;
	}
}

#undef V
