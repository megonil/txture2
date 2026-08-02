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
