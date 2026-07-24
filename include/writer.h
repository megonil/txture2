#ifndef TXTURE2_WRITER_h
#define TXTURE2_WRITER_h

#include "color.h"
#include "image.h"

#define OSCS O (Simplex, simplex_prepare, simplex, simplex_clear)

typedef enum
{
#define O(Name, prep, osc, clear) Name,
	OSCS
#undef O
} GeneratorType;

void write_colors (Colors, GeneratorType, ImageProps);

#endif // !TXTURE2_WRITER_h
