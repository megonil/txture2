#ifndef TXTURE2_WRITER_h
#define TXTURE2_WRITER_h

#include "color.h"
#include "image.h"

#define OSCS O (simplex, simplex_prepare, simplexf, simplex_clear)

typedef enum {
#define O(Name, prep, osc, clear) Name,
	OSCS
#undef O
} GeneratorType;

static const char* const genstrs[] = {
#define O(Name, prep, osc, clr) #Name,
	OSCS
#undef O
};

#define gen_to_str(g) genstrs[g]

void write_colors (Colors, GeneratorType, ImageProps);

GeneratorType
writer_from_string (const char*);

#endif // !TXTURE2_WRITER_h
