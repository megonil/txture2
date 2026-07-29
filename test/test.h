#ifndef TXTURE_TEST_H
#define TXTURE_TEST_H

#include "unity.h"

#include <stddef.h>
#include <stdint.h>

#define ARRAY                                                             \
	TEST (array_init, "array")                                            \
	TEST (array_push_one, "array")                                        \
	TEST (array_push_many, "array")                                       \
	TEST (array_grow, "array")                                            \
	TEST (array_pop, "array")                                             \
	TEST (array_struct, "array")                                          \
	TEST (array_char, "array")                                            \
	TEST (string_clear, "array")                                          \
	TEST (array_empty, "array")                                           \
	TEST (array_large, "array")

#define LEXER                                                             \
	TEST (lexer_integer, "lexer")                                         \
	TEST (lexer_float, "lexer")                                           \
	TEST (lexer_signed, "lexer")                                          \
	TEST (lexer_eof, "lexer")                                             \
	TEST (lexer_exponential, "lexer")                                     \
	TEST (lexer_spaces, "lexer")                                          \
	TEST (lexer_comments, "lexer")                                        \
	TEST (lexer_id, "lexer")                                              \
	TEST (lexer_keyword, "lexer")

#define TABLE                                                             \
	TEST (Table_init_free, "table")                                       \
	TEST (Table_insert_get_update_remove, "table")                        \
	TEST (StringTable, "table")                                           \
	TEST (NumTable, "table")

#define CHUNK                                                             \
	TEST (chunk, "chunk")                                                 \
	TEST (chunk_byte, "chunk")                                            \
	TEST (chunk_constant, "chunk")                                        \
	TEST (chunk_expanded, "chunk")

#define TESTS                                                             \
	ARRAY                                                                 \
	LEXER                                                                 \
	TABLE                                                                 \
	CHUNK

#define testname(Name) test_##Name
#define testfn(Name) void testname (Name) (void)

#define TEST(Name, Group) testfn (Name);
TESTS
#undef TEST

#endif // !TXTURE_TEST_H
