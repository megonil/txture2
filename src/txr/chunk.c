#include "txr/chunk.h"

#include "array.h"
#include "txr/opcode.h"
#include "txr/value.h"
#include "utils.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void
chunk_init (Chunk* chunk)
{
	chunk->code		 = array (opcode);
	chunk->constants = array (tvalue);
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
raw_constant (Chunk* chunk, size_t index)
{
	if (index > UINT8_MAX) {
		to_u24 (index);
		bytes (chunk, Expand, Const, high, mid, low);
	} else {
		bytes (chunk, Const, index);
	}
}

size_t
constant (Chunk* chunk, tvalue v)
{
	push (chunk->constants, v);

	size_t idx = len (chunk->constants) - 1;
	raw_constant (chunk, idx);

	return idx;
}

void
chunk_free (Chunk* chunk)
{
	array_free (chunk->constants);
	array_free (chunk->code);
}

static opcode*
disassemble_const (Chunk* chunk, opcode* code, int expanded)
{
	code++;
	uint32_t	index;
	const char* additional_tag = "";
	if (expanded) {
		uint8_t high = *code++;
		uint8_t mid	 = *code++;
		uint8_t low	 = *code++;
		index		 = from_u24 ();

		additional_tag = "expanded";
	} else {
		index = *code++;
	}

	tvalue v = chunk->constants[index];

	println ("%s const %u (%g)", additional_tag, index, v);

	return code;
}

#define B(Variant, Ch, Str, Prec)                                         \
	case Variant: println (" " Str " (%c)", Ch); break;

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
	case Variant: println (" " Str " (%c)", Ch); break;

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
		printf("%zu ", code - chunk->code);

		switch (*code) {
		case Expand: expanded = 1; code++; println("expand"); continue;
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
