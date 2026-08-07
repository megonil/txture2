#ifndef TXTURE2_TXR_CHUNK_H
#define TXTURE2_TXR_CHUNK_H

#include "txr/opcode.h"
#include "txr/value.h"

#include <stddef.h>
#include <stdint.h>

typedef struct {
	tvalue* constants;
	opcode* code;
	char**	strings_to_free;
} Chunk;

Chunk*
make_chunk ();

void
byte (Chunk* chunk, uint8_t byte);

#define bytes(...) emitbytes (chunk, ##__VA_ARGS__, -1)

void
emitbytes (Chunk* chunk, ...);

void
instruction_with_index (Chunk* chunk, opcode instruction, size_t index);

void
instruction_constant (Chunk* chunk, opcode instruction, tvalue v);

size_t
write_constant (Chunk* chunk, tvalue v);

void
raw_constant (Chunk* chunk, size_t index);

size_t
constant (Chunk* chunk, tvalue v);

void
disassemble (Chunk* chunk);

void
chunk_free (Chunk* chunk);

#endif // !TXTURE2_TXR_CHUNK_H
