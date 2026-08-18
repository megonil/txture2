#ifndef TXTURE_TEST_H
#define TXTURE_TEST_H

#include "unity.h"

#include <stddef.h>
#include <stdint.h>

#define ARRAY                                                             \
	TEST (array_pop, "array")                                             \
	TEST (array_char, "array")                                            \
	TEST (array_init, "array")                                            \
	TEST (array_push, "array")                                            \
	TEST (array_copy, "array")                                            \
	TEST (array_grow, "array")                                            \
	TEST (array_empty, "array")                                           \
	TEST (array_clear, "array")                                           \
	TEST (array_large, "array")                                           \
	TEST (array_struct, "array")                                          \
	TEST (array_push_many, "array")                                       \
	TEST (array_multiple_push, "array")

#define LEXER                                                             \
	TEST (lexer_id, "lexer")                                              \
	TEST (lexer_eof, "lexer")                                             \
	TEST (lexer_float, "lexer")                                           \
	TEST (lexer_signed, "lexer")                                          \
	TEST (lexer_binary, "lexer")                                          \
	TEST (lexer_spaces, "lexer")                                          \
	TEST (lexer_integer, "lexer")                                         \
	TEST (lexer_keyword, "lexer")                                         \
	TEST (lexer_comments, "lexer")                                        \
	TEST (lexer_character, "lexer")                                       \
	TEST (lexer_exponential, "lexer")                                     \
	TEST (lexer_complex_numbers, "lexer")                                 \
	TEST (lexer_preprocess_alias, "lexer")                                \
	TEST (lexer_preprocess_macrofn, "lexer")                              \
	TEST (lexer_multiplechars, "lexer")                                   \
	TEST (lexer_ambiguous_multiplechars, "lexer")                         \
	TEST (lexer_preprocess_macrofn_depth, "lexer")                        \
	TEST (lexer_preprocess_macrofn_arguments, "lexer")                    \
	TEST (lexer_preprocess_macrofn_arguments_depth, "lexer")

#define TABLE                                                             \
	TEST (table, "table")                                                 \
	TEST (table_all, "table")                                             \
	TEST (num_table, "table")                                             \
	TEST (string_table, "table")

#define CHUNK                                                             \
	TEST (chunk, "chunk")                                                 \
	TEST (chunk_byte, "chunk")                                            \
	TEST (chunk_constant, "chunk")                                        \
	TEST (chunk_expanded, "chunk")

#define PARSER                                                            \
	TEST (parser, "parser")                                               \
	TEST (parser_pow, "parser")                                           \
	TEST (parser_call, "parser")                                          \
	TEST (parser_number, "parser")                                        \
	TEST (parser_simple_expr, "parser")                                   \
	TEST (parser_complex_expr, "parser")                                  \
	TEST (parser_variable_get, "parser")                                  \
	TEST (parser_variable_set, "parser")                                  \
	TEST (parser_pragmas_null, "parser")                                  \
	TEST (parser_pragmas_correct, "parser")                               \
	TEST (parser_multiple_assigns, "parser")                              \
	TEST (parser_binary_additional, "parser")                             \
	TEST (parser_more_complex_expr, "parser")

#define VMT                                                               \
	TEST (vm, "vm")                                                       \
	TEST (vm_call, "vm")                                                  \
	TEST (vm_unary, "vm")                                                 \
	TEST (vm_binary, "vm")                                                \
	TEST (vm_constant, "vm")                                              \
	TEST (vm_calculator, "vm")                                            \
	TEST (vm_correct_power, "vm")                                         \
	TEST (vm_unknown_variable, "vm")                                      \
	TEST (vm_variable_setload, "vm")                                      \
	TEST (vm_variable_reassign, "vm")                                     \
	TEST (vm_variable_reassign_self, "vm")                                \
	TEST (vm_wrong_argument_quantity, "vm")                               \
	TEST (vm_unknown_builtin_function, "vm")

#define TESTS                                                             \
	ARRAY                                                                 \
	TABLE                                                                 \
	CHUNK                                                                 \
	LEXER                                                                 \
	PARSER                                                                \
	VMT

#define testname(Name) test_##Name
#define testfn(Name) void testname (Name) (void)

#define TEST(Name, Group) testfn (Name);
TESTS
#undef TEST

#define t(f) void testname (f) (void)

#endif // !TXTURE_TEST_H
