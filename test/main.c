#include "test.h"
#include "unity.h"
#include "unity_internals.h"

void
setUp (void)
{
}

void
tearDown (void)
{
}

int
main (void)
{
	UNITY_BEGIN ();
	// array and string(char array)
	RUN_TEST (test_array_init);
	RUN_TEST (test_array_push_one);
	RUN_TEST (test_array_push_many);
	RUN_TEST (test_array_pop);
	RUN_TEST (test_array_large);
	RUN_TEST (test_array_empty);
	RUN_TEST (test_array_struct);
	RUN_TEST (test_array_grow);
	RUN_TEST (test_array_char);
	RUN_TEST (test_string_clear);

	// table
	RUN_TEST (test_Table_init_free);
	RUN_TEST (test_Table_insert_get_update_remove);
	RUN_TEST (test_StringTable);
	RUN_TEST (test_NumTable);

	return UNITY_END ();
}
