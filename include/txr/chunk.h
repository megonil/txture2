#ifndef TXTURE2_TXR_CHUNK_H
#define TXTURE2_TXR_CHUNK_H

#include "txr/opcode.h"
#include "txr/value.h"
#include "utils.h"

#include <stddef.h>
#include <stdint.h>

typedef struct {
	tvalue* constants;
	opcode* code;
	char**	strings_to_free;
	uint*	lines;
	uint	last_line;
} Chunk;

Chunk*
make_chunk ();

void
byte (Chunk* chunk, uint8_t byte, uint line);

#define bytes(line, ...) emitbytes (chunk, line, ##__VA_ARGS__, -1)

void
emitbytes (Chunk* chunk, uint line, ...);

void
instruction_with_index (
	Chunk* chunk,
	opcode instruction,
	size_t index,
	uint   line);

void
instruction_constant (
	Chunk* chunk,
	opcode instruction,
	tvalue v,
	uint   line);

size_t
write_constant (Chunk* chunk, tvalue v);

void
raw_constant (Chunk* chunk, size_t index, uint line);

size_t
constant (Chunk* chunk, tvalue v, uint line);

void
disassemble (Chunk* chunk);

void
chunk_free (Chunk* chunk);

#endif // !TXTURE2_TXR_CHUNK_H
