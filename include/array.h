#ifndef TXTURE2_arrayh
#define TXTURE2_arrayh

#include <stddef.h>
typedef void (PrintFunction (void*));

typedef struct {
	size_t item_size;
	size_t len;
	size_t cap;
} ArrayHeader;

#define CAP_INITIAL 8

#define array(T) (T*) array_init (sizeof (T), CAP_INITIAL)
#define arrayc(T, cap) (T*) array_init (sizeof (T), cap)
#define arrayh(arr) ((ArrayHeader*) (arr) - 1)

#define cap(arr) arrayh (arr)->cap
#define len(arr) arrayh (arr)->len
#define isize(arr) arrayh (arr)->item_size

#define array_empty(arr) (len (arr) == 0)
#define array_reserve(arr, items) arr = array_resize (arr, items)

#define pushj(arr, val) arr[len (arr)++] = val
#define push(arr, val)                                                    \
	do {                                                                  \
		array_reserve (arr, 1);                                           \
		pushj (arr, val);                                                 \
	} while (0)

#define array_inc(arr) len (arr)++

#define pop(arr) arr[--len (arr)]
#define last(arr) arr[len (arr) - 1]
#define foreach(arr, name) for (size_t name = 0; name < len (arr); ++name)

#define string() array (char)

/// Init Array
void*
array_init (size_t item_size, size_t capacity);

/// Resize Array if needed so it can fit another n elements
void*
array_resize (void* array, size_t n);

void
array_free (void* array);

/// Invoke print function on all elements
void
array_print (void* array, PrintFunction fn);

/// Set all bytes of array to 0
void
array_clear (void* array);

#endif
