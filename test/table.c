#include "table.h"

#include "test.h"

void
test_Table_init_free (void)
{
	Table table;
	TableInit (&table);

	TEST_ASSERT_NOT_NULL (table.entries);
	TEST_ASSERT_TRUE (table.cap > 0);
	TEST_ASSERT_EQUAL_UINT64 (0, (unsigned long long) table.size);

	TableFree (&table);

	TEST_ASSERT_NULL (table.entries);
	TEST_ASSERT_EQUAL_UINT64 (0, (unsigned long long) table.cap);
	TEST_ASSERT_EQUAL_UINT64 (0, (unsigned long long) table.size);
}

void
test_Table_insert_get_update_remove (void)
{
	Table table;
	TableInit (&table);

	TEST_ASSERT_EQUAL_INT (1, TableInsert (&table, "alpha", 10));
	TEST_ASSERT_TRUE (TableContains (&table, "alpha"));

	int* value = TableGet (&table, "alpha");
	TEST_ASSERT_NOT_NULL (value);
	TEST_ASSERT_EQUAL_INT (10, *value);

	TEST_ASSERT_EQUAL_INT (0, TableInsert (&table, "alpha", 42));
	value = TableGet (&table, "alpha");
	TEST_ASSERT_NOT_NULL (value);
	TEST_ASSERT_EQUAL_INT (42, *value);

	TableRemove (&table, "alpha");
	TEST_ASSERT_FALSE (TableContains (&table, "alpha"));
	TEST_ASSERT_NULL (TableGet (&table, "alpha"));

	TEST_ASSERT_EQUAL_INT (1, TableInsert (&table, "alpha", 77));
	value = TableGet (&table, "alpha");
	TEST_ASSERT_NOT_NULL (value);
	TEST_ASSERT_EQUAL_INT (77, *value);

	TableFree (&table);
}

void
test_StringTable (void)
{
	StringTable table;
	StringTableInit (&table);

	TEST_ASSERT_EQUAL_INT (1, StringTableInsert (&table, "name", "Alice"));
	TEST_ASSERT_TRUE (StringTableContains (&table, "name"));

	const char** value = StringTableGet (&table, "name");
	TEST_ASSERT_NOT_NULL (value);
	TEST_ASSERT_NOT_NULL (*value);
	TEST_ASSERT_EQUAL_STRING ("Alice", *value);

	TEST_ASSERT_EQUAL_INT (0, StringTableInsert (&table, "name", "Bob"));
	value = StringTableGet (&table, "name");
	TEST_ASSERT_NOT_NULL (value);
	TEST_ASSERT_EQUAL_STRING ("Bob", *value);

	TEST_ASSERT_EQUAL_INT (
		1, StringTableInsert (&table, "city", "Stockholm"));
	TEST_ASSERT_TRUE (StringTableContains (&table, "city"));

	StringTableRemove (&table, "city");
	TEST_ASSERT_FALSE (StringTableContains (&table, "city"));
	TEST_ASSERT_NULL (StringTableGet (&table, "city"));

	TEST_ASSERT_EQUAL_INT (
		1, StringTableInsert (&table, "city", "Uppsala"));
	value = StringTableGet (&table, "city");
	TEST_ASSERT_NOT_NULL (value);
	TEST_ASSERT_EQUAL_STRING ("Uppsala", *value);

	StringTableFree (&table);
}

void
test_NumTable (void)
{
	NumTable table;
	NumTableInit (&table);

	TEST_ASSERT_EQUAL_INT (1, NumTableInsert (&table, 7, 70));
	TEST_ASSERT_TRUE (NumTableContains (&table, 7));

	int* value = NumTableGet (&table, 7);
	TEST_ASSERT_NOT_NULL (value);
	TEST_ASSERT_EQUAL_INT (70, *value);

	TEST_ASSERT_EQUAL_INT (0, NumTableInsert (&table, 7, 77));
	value = NumTableGet (&table, 7);
	TEST_ASSERT_NOT_NULL (value);
	TEST_ASSERT_EQUAL_INT (77, *value);

	TEST_ASSERT_EQUAL_INT (1, NumTableInsert (&table, 13, 130));
	TEST_ASSERT_TRUE (NumTableContains (&table, 13));

	NumTableRemove (&table, 13);
	TEST_ASSERT_FALSE (NumTableContains (&table, 13));
	TEST_ASSERT_NULL (NumTableGet (&table, 13));

	TEST_ASSERT_EQUAL_INT (1, NumTableInsert (&table, 13, 260));
	value = NumTableGet (&table, 13);
	TEST_ASSERT_NOT_NULL (value);
	TEST_ASSERT_EQUAL_INT (260, *value);

	NumTableFree (&table);
}
