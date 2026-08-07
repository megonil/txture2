#include "utils.h"

#include <errno.h>
#include <stdlib.h>

#define parse(f)                                                          \
	char* end;                                                            \
	errno = 0;                                                            \
	*ok	  = 1;                                                            \
                                                                          \
	const double i = f (b, &end);                                         \
	if (b == end) *ok = 0;                                                \
                                                                          \
	const int range_error = errno == ERANGE;                              \
	if (range_error) *ok = 0;                                             \
                                                                          \
	return i;

double
parse_double (const char* b, int* ok)
{ parse (strtod) }

float
parse_float (const char* b, int* ok)
{ parse (strtof) }

char* const
readfile (const char* filename)
{
	FILE* file = fopen (filename, "rb");

	if (file == NULL) error ("Couldn't open file");

	fseek (file, 0L, SEEK_END);
	size_t file_size = ftell (file);

	char* value = (char*) malloc (sizeof (char) * (file_size + 1));

	rewind (file);

	size_t bytes_read = fread (value, sizeof (char), file_size, file);

	if (bytes_read < file_size) {
		fprintf (stderr, "Could not open file");
		exit (1);
	}

	fclose (file);
	value[file_size + 1] = '\0';

	return value;
}
