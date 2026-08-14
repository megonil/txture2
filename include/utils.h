#ifndef TXTURE2_UTILS_H
#define TXTURE2_UTILS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned uint;
typedef uint64_t pxpos;

#define fprintln(fd, fmt, ...) fprintf (fd, fmt "\n", ##__VA_ARGS__)
#define eprintln(fmt, ...) fprintln (stderr, fmt, ##__VA_ARGS__)
#define println(fmt, ...) fprintln (stdout, fmt, ##__VA_ARGS__)

#define error(fmt, ...)                                                   \
	do {                                                                  \
		eprintln ("error: " fmt, ##__VA_ARGS__);                          \
		exit (1);                                                         \
	} while (0)

#define note(fmt, ...) fprintln (stderr, "note: " fmt, ##__VA_ARGS__)
#define warn(fmt, ...) fprintln (stderr, "warning: " fmt, ##__VA_ARGS__)

#define streq(a, b) strcmp (a, b) == 0

#ifdef TXTURE2_DEBUG
#define debug(fmt, ...) eprintln (fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...) ((void) 0)
#endif

double
parse_double (const char* b, int* ok);

float
parse_float (const char* b, int* result);

char* const
readfile (const char* filename);

#define from_u24()                                                        \
	((uint32_t) high << 16) | ((uint32_t) mid << 8) | (uint32_t) low

#define to_u24(v)                                                         \
	uint8_t low	 = (v) & 0xFF;                                            \
	uint8_t mid	 = ((v) >> 8) & 0xFF;                                     \
	uint8_t high = ((v) >> 16) & 0xFF

#ifdef __GNUC__ // GCC 4.8+, Clang, Intel and other compilers compatible
				// with GCC (-std=c++0x or above)
[[noreturn]] inline __attribute__ ((always_inline)) void
unreachable ()
{ __builtin_unreachable (); }
#elif defined(_MSC_VER) // MSVC
[[noreturn]] __forceinline void
unreachable ()
{ __assume (false); }
#else					// ???
inline void
unreachable ()
{ abort (); }
#endif

#endif // !TXTURE2_UTILS_H
