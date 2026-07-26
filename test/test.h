#ifndef TXTURE_TEST_H
#define TXTURE_TEST_H

#include "unity.h"

#include <stddef.h>
#include <stdint.h>

void
setUp (void);
void
tearDown (void);

void
test_array_init (void);
void
test_array_push_one (void);
void
test_array_push_many (void);
void
test_array_grow (void);
void
test_array_pop (void);
void
test_array_struct (void);
void
test_array_char (void);
void
test_string_clear (void);
void
test_array_empty (void);
void
test_array_large (void);

void
test_Table_init_free (void);
void
test_Table_insert_get_update_remove (void);
void
test_StringTable (void);
void
test_NumTable (void);

void
test_lexer_integer (void);
void
test_lexer_float (void);
void
test_lexer_signed (void);
void
test_lexer_eof (void);
void
test_lexer_exponential (void);
void
test_lexer_spaces (void);
void
test_lexer_comments (void);
void
test_lexer_id ();
void
test_lexer_keyword ();

#endif /* TABLE_TEST_H */
