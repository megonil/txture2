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
	chunk->lines		   = array (uint);
	chunk->last_line	   = 1;

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
	array_free (chunk->lines);

	free (chunk);
}

inline void
byte (Chunk* chunk, uint8_t b, uint line)
{
	push (chunk->code, (opcode) b);

	if (len (chunk->lines) == 0 || line != chunk->last_line) {
		push (chunk->lines, 1);
		chunk->last_line = line;
	} else {
		last (chunk->lines)++;
	}
}

void
emitbytes (Chunk* chunk, uint line, ...)
{
	va_list bytes;
	va_start (bytes, line);

	for (int bte; (bte = va_arg (bytes, int)) != -1;) {
		byte (chunk, bte, line);
	}

	va_end (bytes);
}

void
instruction_with_index (
	Chunk* chunk,
	opcode instruction,
	size_t index,
	uint   line)
{
	if (index > UINT8_MAX) {
		to_u24 (index);
		bytes (line, Expand, instruction, high, mid, low);
	} else {
		bytes (line, instruction, index);
	}
}

inline void
instruction_constant (
	Chunk* chunk,
	opcode instruction,
	tvalue v,
	uint   line)
{
	return instruction_with_index (
		chunk, instruction, write_constant (chunk, v), line);
}

inline void
raw_constant (Chunk* chunk, size_t index, uint line)
{ instruction_with_index (chunk, Const, index, line); }

size_t
write_constant (Chunk* chunk, tvalue v)
{
	push (chunk->constants, v);
	return len (chunk->constants) - 1;
}

size_t
constant (Chunk* chunk, tvalue v, uint line)
{
	size_t idx = write_constant (chunk, v);
	raw_constant (chunk, idx, line);

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

	printf ("%sconst %zu (%g)", additional_tag, index, v);

	return code;
}

static opcode*
disassemble_instruction_with_index (
	Chunk*		chunk,
	opcode*		code,
	int			expanded,
	const char* instruction_str)
{
	code++;

	size_t		index  = get_index (&code, expanded);
	const char* string = chunk->strings_to_free[index];

	printf ("%s %zu (%s)", instruction_str, index, string);

	return code;
}

static inline opcode*
disassemble_load (Chunk* chunk, opcode* code, uint expanded)
{
	return disassemble_instruction_with_index (
		chunk, code, expanded, "load");
}

static inline opcode*
disassemble_set (Chunk* chunk, opcode* code, uint expanded)
{
	return disassemble_instruction_with_index (
		chunk, code, expanded, "set");
}

static opcode*
disassemble_call (Chunk* chunk, opcode* code, uint expanded)
{
	code++;
	size_t		s	   = get_index (&code, expanded);
	const char* fnname = chunk->strings_to_free[s];

	uint8_t argc = *code++;
	printf ("call %s(argc = %u)", fnname, argc);

	return code;
}

#define B(Variant, Ch, Str, Prec)                                         \
	case Variant: printf (Str " (%s)", tok_to_string (Ch)); break;

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
	case Variant: printf (Str " (%c)", Ch); break;

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
	opcode* code = chunk->code;
	opcode* end	 = chunk->code + len (chunk->code);
	uint*	line = chunk->lines;
	debug ("CHUNK LINE PTR: %p", line);

	uint remaining	 = 0;
	uint source_line = 0;

	uint expanded = 0;

	while (code < end) {
		if (remaining == 0) {
			remaining = *line++;
			source_line++;
		}

		opcode* last = code;
		printf ("%-7zu ", (size_t) (code - chunk->code));

		// clang-format off
		switch (*code) {
			case Expand: expanded = 1; code++; printf ("expand"); goto next_;
			case Pop: code++; printf ("pop"); break;
			case Load:
				code = disassemble_load(chunk ,code, expanded);
				break;
			case Call:
				code = disassemble_call(chunk, code, expanded);
				break;
			case Set:
				code = disassemble_set(chunk, code, expanded);
				break;
			case Const:
				code	 = disassemble_const (chunk, code, expanded);
				break;

			UNARY_INSTRUCTIONS
				code = disassemble_unary(chunk, code);
				break;

			BINARY_INSTRUCTIONS
				code = disassemble_binary (chunk, code);
				break;
		}
		// clang-format on

		expanded = 0;
	next_:
		remaining -= (uint) (code - last);

		printf ("\033[35G| line=%u\n", source_line);
	}
}

#undef B
#undef U
