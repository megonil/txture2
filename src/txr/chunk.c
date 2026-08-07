#include "txr/chunk.h"

#include "array.h"
#include "txr/opcode.h"
#include "txr/token.h"
#include "txr/value.h"
#include "utils.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

Chunk*
make_chunk ()
{
	Chunk* chunk = malloc (sizeof (Chunk));

	chunk->code			   = array (opcode);
	chunk->constants	   = array (tvalue);
	chunk->strings_to_free = array (char*);

	return chunk;
}

void
chunk_free (Chunk* chunk)
{
	array_free (chunk->constants);
	array_free (chunk->code);

	foreach (chunk->strings_to_free, i) {
		free (chunk->strings_to_free[i]);
	}

	array_free (chunk->strings_to_free);

	free (chunk);
}

inline void
byte (Chunk* chunk, uint8_t b)
{ push (chunk->code, (opcode) b); }

void
emitbytes (Chunk* chunk, ...)
{
	va_list bytes;
	va_start (bytes, chunk);

	for (int bte; (bte = va_arg (bytes, int)) != -1;) {
		byte (chunk, bte);
	}

	va_end (bytes);
}

void
instruction_with_index (Chunk* chunk, opcode instruction, size_t index)
{
	if (index > UINT8_MAX) {
		to_u24 (index);
		bytes (Expand, instruction, high, mid, low);
	} else {
		bytes (instruction, index);
	}
}

inline void
instruction_constant (Chunk* chunk, opcode instruction, tvalue v)
{
	return instruction_with_index (
		chunk, instruction, write_constant (chunk, v));
}

inline void
raw_constant (Chunk* chunk, size_t index)
{ instruction_with_index (chunk, Const, index); }

size_t
write_constant (Chunk* chunk, tvalue v)
{
	push (chunk->constants, v);
	return len (chunk->constants) - 1;
}

size_t
constant (Chunk* chunk, tvalue v)
{
	size_t idx = write_constant (chunk, v);
	raw_constant (chunk, idx);

	return idx;
}

static size_t
get_index (opcode** code, int expanded)
{
	size_t index;
	if (expanded) {
		uint8_t high = *(*code)++;
		uint8_t mid	 = *(*code)++;
		uint8_t low	 = *(*code)++;
		index		 = from_u24 ();
	} else {
		index = *(*code)++;
	}

	return index;
}

static opcode*
disassemble_const (Chunk* chunk, opcode* code, int expanded)
{
	code++;
	size_t		index		   = get_index (&code, expanded);
	const char* additional_tag = expanded ? "expanded " : "";

	tvalue v = chunk->constants[index];

	println ("%sconst %zu (%g)", additional_tag, index, v);

	return code;
}

static opcode*
disassemble_setload (
	Chunk*		chunk,
	opcode*		code,
	int			expanded,
	const char* instruction_str)
{
	code++;
	size_t		index  = get_index (&code, expanded);
	const char* string = chunk->strings_to_free[index];

	println ("%s %zu (%s)", instruction_str, index, string);

	return code;
}

static inline opcode*
disassemble_load (Chunk* chunk, opcode* code, int expanded)
{ return disassemble_setload (chunk, code, expanded, "load"); }

static inline opcode*
disassemble_set (Chunk* chunk, opcode* code, int expanded)
{ return disassemble_setload (chunk, code, expanded, "set"); }

#define B(Variant, Ch, Str, Prec)                                         \
	case Variant: println (Str " (%s)", tok_to_string (Ch)); break;

static opcode*
disassemble_binary (Chunk* chunk, opcode* code)
{
	switch (*code++) {
		BINARY_INSTRUCTIONS
		default: __builtin_unreachable ();
	}

	return code;
}

#define U(Variant, Ch, Str, Op)                                           \
	case Variant: println (Str " (%c)", Ch); break;

static opcode*
disassemble_unary (Chunk* chunk, opcode* code)
{
	switch (*code++) {
		UNARY_INSTRUCTIONS;
		default: __builtin_unreachable ();
	}

	return code;
}

#undef B
#undef U

#define B(Variant, Ch, Str, Prec) case Variant:
#define U(Variant, Ch, Str, Op) case Variant:

void
disassemble (Chunk* chunk)
{
	opcode* code	 = chunk->code;
	int		expanded = 0;

	while (code < chunk->code + len (chunk->code)) {
		// clang-format off
		printf("%-7zu ", code - chunk->code);

		switch (*code) {
			case Expand: expanded = 1; code++; println("expand"); continue;
			case Pop: code++; println("pop"); break;
			case Load:
				code = disassemble_load(chunk ,code, expanded);
				break;
			case Set:
				code = disassemble_set(chunk, code, expanded);
				break;
			case Const:
				code	 = disassemble_const (chunk, code, expanded);
				break;

			UNARY_INSTRUCTIONS
				code = disassemble_unary(chunk, code); break;

			BINARY_INSTRUCTIONS
				code = disassemble_binary (chunk, code);
				break;
			}

		expanded = 0;
		// clang-format on
	}
}

#undef B
#undef U
