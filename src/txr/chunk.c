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
	chunk->constants = array (value);
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

size_t
constant (Chunk* chunk, value v)
{
	push (chunk->constants, v);

	size_t idx = len (chunk->constants) - 1;

	if (idx > UINT8_MAX) {
		to_u24 (idx);
		bytes (chunk, Expand, Const, high, mid, low);
	} else {
		bytes (chunk, Const, idx);
	}

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

	value v = chunk->constants[index];

	println ("%s const %u (%g)", additional_tag, index, v);

	return code;
}

#define B(Variant, Ch, Str)                                               \
	case Variant: printf (Str " (%c)", Ch); break;

static opcode*
disassemble_binary (Chunk* chunk, opcode* code)
{
	switch (*code++) {
		BINARY_INSTRUCTIONS
	default: __builtin_unreachable ();
	}

	return code;
}

#undef B

#define B(Variant, Ch, Str) case Variant:

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

		BINARY_INSTRUCTIONS
			code = disassemble_binary (chunk, code);
			break;
		}

		expanded = 0;
		// clang-format on
	}
}

#undef B
