#include "array.h"

#include "utils.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void*
array_init (size_t itemsize, size_t capacity)
{
	size_t size = itemsize * capacity;
	size += sizeof (ArrayHeader);

	ArrayHeader* header = calloc (1, size);

	if (!header) error ("unable to allocate header");

	header->item_size = itemsize;
	header->cap		  = capacity;
	header->len		  = 0;

	return header + 1;
}

void*
array_resize (void* array, size_t n)
{
	size_t new_length = len (array) + n;
	if (new_length > cap (array)) {
		size_t new_capacity = cap (array);
		while (new_capacity < new_length) { new_capacity *= 2; }

		ArrayHeader* h = realloc (
			arrayh (array),
			sizeof (ArrayHeader) + new_capacity * isize (array));

		if (!h) error ("unable to reallocate header");

		h->cap = new_capacity;
		return h + 1;
	} else {
		return array;
	}
}

void
array_print (void* array, PrintFunction fn)
{
	foreach (array, i) { fn ((char*) array + i * isize (array)); }
}

inline void
array_free (void* array)
{
	assert (array != NULL);

	free (arrayh (array));
}

void
array_clear (void* array)
{
	memset (array, 0, len (array) * isize (array));
	len (array) = 0;
}

static void*
arrcpy (void* dest, const void* src, size_t n, size_t from)
{
	if (n > SIZE_MAX - from) return 0;

	size_t end		  = from + n;
	size_t l		  = len (dest);
	size_t new_length = l;

	if (end > l) {
		dest = array_resize (dest, end - l);
		new_length += end - l;
	}

	memmove ((char*) dest + from * isize (dest), src, n * isize (dest));
	len (dest) = new_length;

	return dest;
}

inline void*
array_copy (void* dest, const void* src, size_t n)
{ return arrcpy (dest, src, n, 0); }

inline void*
array_push_many (void* dest, const void* src, size_t n)
{ return arrcpy (dest, src, n, len (dest)); }
