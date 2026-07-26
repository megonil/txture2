#include "utils.h"

#include <errno.h>
#include <stdlib.h>

float
parse_float (const char* b, int* ok)
{
	char* end;
	errno = 0;
	*ok	  = 1;

	const float i = strtof (b, &end);
	if (b == end) *ok = 0;

	const int range_error = errno == ERANGE;
	if (range_error) *ok = 0;

	return i;
}
