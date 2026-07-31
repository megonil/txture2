#ifndef TXTURE2_TXR_CHUNK_H
#define TXTURE2_TXR_CHUNK_H

#include "txr/opcode.h"
#include "txr/value.h"

#include <stddef.h>
#include <stdint.h>

typedef struct {
	tvalue* constants;
	opcode* code;
} Chunk;

void
chunk_init (Chunk* chunk);

void
byte (Chunk* chunk, uint8_t byte);

#define bytes(chunk, ...) emitbytes (chunk, ##__VA_ARGS__, -1)

void
emitbytes (Chunk* chunk, ...);

void
raw_constant (Chunk* chunk, size_t index);

size_t
constant (Chunk* chunk, tvalue v);

void
disassemble (Chunk* chunk);

void
chunk_free (Chunk* chunk);

#endif // !TXTURE2_TXR_CHUNK_H
