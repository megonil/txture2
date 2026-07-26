#include "array.h"

#include "unity.h"

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

	array_push (arr, 42);

	TEST_ASSERT_EQUAL_UINT (1, len (arr));
	TEST_ASSERT_EQUAL_INT (42, arr[0]);

	array_free (arr);
}

void
test_array_push_many (void)
{
	int* arr = array (int);

	for (int i = 0; i < 100; i++) array_push (arr, i);

	TEST_ASSERT_EQUAL_UINT (100, len (arr));

	for (int i = 0; i < 100; i++) TEST_ASSERT_EQUAL_INT (i, arr[i]);

	array_free (arr);
}

void
test_array_grow (void)
{
	int* arr = arrayc (int, 2);

	TEST_ASSERT_EQUAL_UINT (2, cap (arr));

	array_push (arr, 1);
	array_push (arr, 2);
	array_push (arr, 3);

	TEST_ASSERT_TRUE (cap (arr) >= 3);
	TEST_ASSERT_EQUAL_UINT (3, len (arr));

	array_free (arr);
}

void
test_array_pop (void)
{
	int* arr = array (int);

	array_push (arr, 10);
	array_push (arr, 20);
	array_push (arr, 30);

	TEST_ASSERT_EQUAL_INT (30, array_pop (arr));
	TEST_ASSERT_EQUAL_UINT (2, len (arr));

	TEST_ASSERT_EQUAL_INT (20, array_pop (arr));
	TEST_ASSERT_EQUAL_UINT (1, len (arr));

	TEST_ASSERT_EQUAL_INT (10, array_pop (arr));
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

	array_push (arr, ((Point){1, 2}));
	array_push (arr, ((Point){3, 4}));

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

	array_push (str, 'H');
	array_push (str, 'i');
	array_push (str, '\0');

	TEST_ASSERT_EQUAL_STRING ("Hi", str);

	array_free (str);
}

void
test_string_clear (void)
{
	char* str = string ();

	array_push (str, 'a');
	array_push (str, 'b');
	array_push (str, '\0');

	stringClear (str);

	TEST_ASSERT_EQUAL_UINT (0, len (str));
	TEST_ASSERT_EQUAL_CHAR ('\0', str[0]);

	array_free (str);
}

void
test_array_empty (void)
{
	int* arr = array (int);

	TEST_ASSERT_TRUE (array_empty (arr));

	array_push (arr, 1);

	TEST_ASSERT_FALSE (array_empty (arr));

	array_free (arr);
}

void
test_array_large (void)
{
	int* arr = array (int);

	for (int i = 0; i < 100000; i++) array_push (arr, i);

	TEST_ASSERT_EQUAL_UINT (100000, len (arr));

	for (int i = 0; i < 100000; i++) TEST_ASSERT_EQUAL_INT (i, arr[i]);

	array_free (arr);
}
