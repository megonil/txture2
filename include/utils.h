#ifndef TXTURE2_UTILS_H
#define TXTURE2_UTILS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned uint;
typedef uint64_t pxpos;

#define fprintln(fd, fmt, ...) fprintf (fd, fmt "\n", ##__VA_ARGS__)
#define eprintln(fmt, ...) fprintln (stderr, "error: " fmt, ##__VA_ARGS__)
#define println(fmt, ...) fprintln (stdout, fmt, ##__VA_ARGS__)

#define error(fmt, ...)                                                   \
	eprintln (fmt, ##__VA_ARGS__);                                        \
	exit (1)

#define note(fmt, ...) fprintln (stderr, "note: " fmt, ##__VA_ARGS__)

#define streq(a, b) strcmp (a, b) == 0

float
parse_float (const char* b, int* result);

#endif // !TXTURE2_UTILS_H
