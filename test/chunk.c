#include "txr/chunk.h"

#include "array.h"
#include "test.h"
#include "utils.h"

#include <stdint.h>

#define pro() Chunk* chunk = make_chunk ();

#define epi() chunk_free (chunk)

t (chunk)
{
	pro ();

	epi ();
}

t (chunk_byte)
{
	pro ();

	byte (chunk, 124);
	TEST_ASSERT_EQUAL (124, last (chunk->code));

	epi ();
}

t (chunk_constant)
{
	pro ();

	size_t idx = constant (chunk, 130.5);
	TEST_ASSERT_EQUAL_FLOAT (130.5, chunk->constants[idx]);

	epi ();
}

t (chunk_expanded)
{
	pro ();

	for (uint i = 0; i <= 255; ++i) constant (chunk, i);

	constant (chunk, 25777);

	uint8_t	 low   = pop (chunk->code);
	uint8_t	 mid   = pop (chunk->code);
	uint8_t	 high  = pop (chunk->code);
	uint32_t index = from_u24 ();

	TEST_ASSERT_EQUAL_INT (256, index);
	TEST_ASSERT_EQUAL_FLOAT (25777.0f, chunk->constants[index]);
	TEST_ASSERT_EQUAL_INT (Const, pop (chunk->code));
	TEST_ASSERT_EQUAL_INT (Expand, pop (chunk->code));

	epi ();
}
