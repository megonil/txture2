#include "array.h"

#include "unity.h"

#include <string.h>

void
test_array_init (void)
{
	int* arr = array (int);

	TEST_ASSERT_NOT_NULL (arr);
	TEST_ASSERT_EQUAL_UINT (0, len (arr));
	TEST_ASSERT_EQUAL_UINT (CAP_INITIAL, cap (arr));
	TEST_ASSERT_EQUAL_UINT (sizeof (int), isize (arr));

	array_free (arr);
}

void
test_array_push_one (void)
{
	int* arr = array (int);

	push (arr, 42);

	TEST_ASSERT_EQUAL_UINT (1, len (arr));
	TEST_ASSERT_EQUAL_INT (42, arr[0]);

	array_free (arr);
}

void
test_array_push_many (void)
{
	int* arr = array (int);

	for (int i = 0; i < 100; i++) push (arr, i);

	TEST_ASSERT_EQUAL_UINT (100, len (arr));

	for (int i = 0; i < 100; i++) TEST_ASSERT_EQUAL_INT (i, arr[i]);

	array_free (arr);
}

void
test_array_grow (void)
{
	int* arr = arrayc (int, 2);

	TEST_ASSERT_EQUAL_UINT (2, cap (arr));

	push (arr, 1);
	push (arr, 2);
	push (arr, 3);

	TEST_ASSERT_TRUE (cap (arr) >= 3);
	TEST_ASSERT_EQUAL_UINT (3, len (arr));

	array_free (arr);
}

void
test_array_pop (void)
{
	int* arr = array (int);

	push (arr, 10);
	push (arr, 20);
	push (arr, 30);

	TEST_ASSERT_EQUAL_INT (30, pop (arr));
	TEST_ASSERT_EQUAL_UINT (2, len (arr));

	TEST_ASSERT_EQUAL_INT (20, pop (arr));
	TEST_ASSERT_EQUAL_UINT (1, len (arr));

	TEST_ASSERT_EQUAL_INT (10, pop (arr));
	TEST_ASSERT_EQUAL_UINT (0, len (arr));

	array_free (arr);
}

typedef struct {
	int x;
	int y;
} Point;

void
test_array_struct (void)
{
	Point* arr = array (Point);

	push (arr, ((Point){1, 2}));
	push (arr, ((Point){3, 4}));

	TEST_ASSERT_EQUAL_INT (1, arr[0].x);
	TEST_ASSERT_EQUAL_INT (2, arr[0].y);

	TEST_ASSERT_EQUAL_INT (3, arr[1].x);
	TEST_ASSERT_EQUAL_INT (4, arr[1].y);

	array_free (arr);
}

void
test_array_char (void)
{
	char* str = string ();

	push (str, 'H');
	push (str, 'i');
	push (str, '\0');

	TEST_ASSERT_EQUAL_STRING ("Hi", str);

	array_free (str);
}

void
test_string_clear (void)
{
	char* str = string ();

	push (str, 'a');
	push (str, 'b');
	push (str, '\0');

	array_clear (str);
	push (str, 'a');

	TEST_ASSERT_EQUAL_STRING ("a", str);

	array_free (str);
}

void
test_array_empty (void)
{
	int* arr = array (int);

	TEST_ASSERT_TRUE (array_empty (arr));

	push (arr, 1);

	TEST_ASSERT_FALSE (array_empty (arr));

	array_free (arr);
}

void
test_array_large (void)
{
	int* arr = array (int);

	for (int i = 0; i < 100000; i++) push (arr, i);

	TEST_ASSERT_EQUAL_UINT (100000, len (arr));

	for (int i = 0; i < 100000; i++) TEST_ASSERT_EQUAL_INT (i, arr[i]);

	array_free (arr);
}
