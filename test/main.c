#include "test.h"
#include "unity.h"
#include "unity_internals.h"

#include <string.h>

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
	static const char* current_group = "";

#define TEST(Name, Group)                                                 \
	do {                                                                  \
		if (strcmp (current_group, Group) != 0) {                         \
			current_group = Group;                                        \
			printf ("\n=== %s ===\n", Group);                             \
		}                                                                 \
		RUN_TEST (test_##Name);                                           \
	} while (0);
	TESTS

#undef TEST

	return UNITY_END ();
}
