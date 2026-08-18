#include "txr/parser.h"

#include "array.h"
#include "test.h"
#include "txr/chunk.h"
#include "txr/pragmas.h"
#include "txr/vmbuiltin.h"
#include "unity.h"
#include "utils.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define pro(source)                                                       \
	Chunk*	chunk = make_chunk ();                                        \
	Parser* p	  = make_parser (source, "source", chunk);

#define s(i) BUILTIN_VARIABLES_N + i

#define _parse() parse (p)
#define at(array, i) array[(i)]
#define cat(i) at (chunk->code, i)
#define kat(i) at (chunk->constants, i)
#define sat(i) at (chunk->strings_to_free, s (i))

#define assert_constant(v, i) TEST_ASSERT_EQUAL_FLOAT (v, kat (i))
#define assert_string(v, i) TEST_ASSERT_EQUAL_STRING (v, sat (i))

#define epi()                                                             \
	parser_free (p);                                                      \
	chunk_free (chunk);

void
assert_pragmas_impl (const PragmaKind* expected, size_t n)
{
	// number of bits
	TEST_ASSERT_LESS_OR_EQUAL (sizeof (pragmas) * 8, n);

	uint x = 0;

	for (size_t i = 0; i < n; ++i) { x |= (1 << expected[i]); }

	TEST_ASSERT_EQUAL_UINT (x, pragmas);
}

void
assert_bytecode_impl (
	const Chunk*   chunk,
	const uint8_t* expected,
	size_t		   n)
{
	TEST_ASSERT_EQUAL (n, len (chunk->code));

	for (size_t i = 0; i < n; ++i) {
		if (chunk->code[i] != expected[i]) {
			eprintln (
				"bytecode mismatch at %zu: expected %u got %u\n",
				i,
				expected[i],
				chunk->code[i]);
		}
	}
}

#define assert_bytecode(...)                                              \
	assert_bytecode_impl (                                                \
		chunk,                                                            \
		(uint8_t[]){__VA_ARGS__},                                         \
		sizeof ((uint8_t[]){__VA_ARGS__}) / sizeof (uint8_t))

#define assert_pragmas(...)                                               \
	assert_pragmas_impl (                                                 \
		(PragmaKind[]){__VA_ARGS__},                                      \
		sizeof ((PragmaKind[]){__VA_ARGS__}) / sizeof (PragmaKind))

#define assert_no_pragmas() TEST_ASSERT_EQUAL_UINT (0, pragmas)

t (parser)
{
	// check for correct initialization
	// and free
	// and loop exiting
	pro ("");
	_parse ();

	epi ();
}

t (parser_number)
{
	pro ("1234.5");
	_parse ();

	assert_bytecode (Const, 0, Pop);
	assert_constant (1234.5f, 0);

	epi ();
}

t (parser_simple_expr)
{
	pro ("123.5 + 5690e-10");
	_parse ();

	assert_bytecode (Const, 0, Const, 1, Add, Pop);
	assert_constant (123.5f, 0);
	assert_constant (5690e-10f, 1);

	epi ();
}

t (parser_more_complex_expr)
{
	pro ("-1234.56 + 12345 * 500");
	_parse ();

	assert_bytecode (Const, 0, Const, 1, Const, 2, Mul, Add, Pop);
	assert_constant (-1234.56, 0);
	assert_constant (12345, 1);
	assert_constant (500, 2);

	epi ();
}

t (parser_complex_expr)
{
	pro ("-(!(1) + 500.00 * 000.00001e-10) + -(500.123*200)");
	_parse ();

	assert_bytecode (
		Const,
		0,
		Not,
		Const,
		1,
		Const,
		2,
		Mul,
		Add,
		Neg,
		Const,
		3,
		Const,
		4,
		Mul,
		Neg,
		Add,
		Pop);

	assert_constant (1, 0);
	assert_constant (500, 1);
	assert_constant (000.00001e-10, 2);
	assert_constant (500.123, 3);
	assert_constant (200, 4);

	epi ()
}

t (parser_pow)
{
	pro ("2 ^ (3 - 6) * 10");
	_parse ();

	assert_bytecode (
		Const, 0, Const, 1, Const, 2, Sub, Pow, Const, 3, Pow, Pop);
	assert_constant (2, 0);
	assert_constant (3, 1);
	assert_constant (6, 2);
	assert_constant (10, 3);

	epi ();
}

t (parser_binary_additional)
{
	pro ("123 + 56879 * 247 + 2.5 ^ (2 + 5.3 * 1.15)");
	_parse ();

	assert_bytecode (
		Const,
		0,
		Const,
		1,
		Const,
		2,
		Mul,
		Add,
		Const,
		3,
		Const,
		4,
		Const,
		5,
		Const,
		6,
		Mul,
		Add,
		Pow,
		Add,
		Pop);

	epi ();
}

t (parser_variable_set)
{
	pro ("abcdef = 1238 + 150.56");
	_parse ();

	assert_bytecode (Const, 0, Const, 1, Add, Set, 0, Pop);

	epi ();
}

t (parser_variable_get)
{
	pro ("abcdef = 1238 + 150.56 abcdef");
	_parse ();

	assert_bytecode (Const, 0, Const, 1, Add, Set, 0, Load, 0, Pop, Pop);

	epi ();
}

t (parser_pragmas_correct)
{
	pro ("pragma expand_r\npragma mono");
	_parse ();

	assert_pragmas (PrExpandR, PrMono);

	epi ();
}

t (parser_pragmas_null)
{
	pro ("pragma expand\npragma expand_r");
	_parse ();

	assert_no_pragmas ();

	epi ();
}

t (parser_call)
{
	pro ("sin(1230)");
	_parse ();

	assert_bytecode (Const, 0, Call, s (0), 1, Pop);
	assert_constant (1230.0, 0);
	assert_string ("sin", 0);

	epi ();
}

t (parser_multiple_assigns)
{
	pro ("a = b = c = 100");
	_parse ();

	assert_bytecode (Const, 0, Set, s (0), Set, s (1), Set, s (2), Pop);
	assert_constant (100.0, 0);

	epi ();
}
