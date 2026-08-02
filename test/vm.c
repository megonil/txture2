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
	VM* vm = make_vm ()

#define exec() VMResult result = execute (vm, &chunk)

#define nofail(source)                                                    \
	pro (source);                                                         \
	exec ();                                                              \
	TEST_ASSERT_EQUAL (VMOK, result.code.kind)

#define fail(source, err)                                                 \
	pro (source);                                                         \
	exec ();                                                              \
	TEST_ASSERT_EQUAL (err, result.code.kind);                            \
	epi ()

#define epi()                                                             \
	chunk_free (&chunk);                                                  \
	parser_free (parser);                                                 \
	vm_free (vm)

#define assert_stack_at(v, i)                                             \
	TEST_ASSERT_EQUAL_DOUBLE (v, vm->stack[len (vm->stack) - 1 - i])

static tvalue
get_variable_value (VM* vm, const char* name)
{
	tvalue* v = VariableTableGet (&vm->variables, name);
	TEST_ASSERT_NOT_NULL (v);

	return *v;
}

#define variable(name) get_variable_value (vm, name)

#define assert_top(v) assert_stack_at (v, 0)
#define assert_variable(varname, varval)                                  \
	TEST_ASSERT_EQUAL_DOUBLE ((varval), variable (varname))

t (vm)
{ nofail (""); }

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

	assert_top (257.0);

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

	assert_top (123.0 + 56879.0 * 247.0 + pow (2.5, 2 + 5.3 * 1.15));

	epi ();
}

t (vm_variable_setload)
{
	pro ("abcdef = 123.0 + 5712412894 abcdef");
	exec ();
	assert_variable ("abcdef", 123.0 + 5712412894.0);
	assert_top (variable ("abcdef"));

	epi ();
}

t (vm_variable_reassign)
{
	pro ("abcdef = 1500 abcdef = 50");
	exec ();

	assert_variable ("abcdef", 50.0);

	epi ();
}

t (vm_variable_reassign_self)
{
	pro ("abcdef = 1500 abcdef = abcdef + 50");
	exec ();

	assert_variable ("abcdef", 1500.0 + 50.0);

	epi ();
}

t (vm_unknown_variable)
{ fail ("hello", UnknownVariable); }
