#include "txr/vm.h"

#include "array.h"
#include "test.h"
#include "txr/chunk.h"
#include "txr/parser.h"

#define pro(source)                                                       \
	Chunk chunk;                                                          \
	chunk_init (&chunk);                                                  \
	Parser* parser = make_parser (source, "source", &chunk);              \
	TEST_ASSERT_NOT_EQUAL (0, parse (parser));                            \
	VM* vm = make_vm ();

#define exec() execute (vm, &chunk)

#define epi()                                                             \
	chunk_free (&chunk);                                                  \
	parser_free (parser);                                                 \
	vm_free (vm);

#define assert_stack_at(v, i)                                             \
	TEST_ASSERT_EQUAL_FLOAT (v, vm->stack[len (vm->stack) - 1 - i])

#define assert_top(v) assert_stack_at (v, 0)

t (vm)
{
	pro ("");
	exec ();

	epi ();
}

t (vm_binary)
{
	pro ("123.5789 + 5839523e-10");
	exec ();
	assert_top (123.5789 + 5839523e-10);

	epi ();
}

t (vm_unary)
{
	pro ("-10.56789");
	exec ();
	assert_top (-10.56789);

	epi ();
}

t (vm_constant)
{
	pro ("");
	for (size_t i = 0; i <= 257; ++i) constant (&chunk, i);
	exec ();

	assert_top (257.0f);

	epi ();
}

t (vm_correct_power)
{
	pro ("2 ^ 2 ^ 2");
	exec ();

	assert_top (16);

	epi ();
}

t (vm_calculator)
{
	pro ("123 + 56879 * 247 + 2.5 ^ (2 + 5.3 * 1.15)");
	exec ();

	assert_top (123.0f + 56879.0f * 247.0f + pow (2.5, 2 + 5.3 * 1.15));

	epi ();
}
