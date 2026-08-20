#ifndef TXTURE2_WRITER_h
#define TXTURE2_WRITER_h

#include "color.h"
#include "image.h"

#include <stdbool.h>

#define OSCS O (simplex, simplex_prepare, 0, simplexf, simplex_clear, 0)

typedef enum {
#define O(Name, prep, prep_thr, osc, clear, clear_thr) Name,
	OSCS
#undef O
		TXRScript
} GeneratorType;

static const char* const genstrs[] = {
#define O(Name, prep, prep_thr, osc, clr, clr_thr) #Name,
	OSCS
#undef O
};

#define gen_to_str(g) genstrs[g]

bool write_colors (Colors, GeneratorType, ImageProps);

GeneratorType
writer_from_string (const char*);

#endif // !TXTURE2_WRITER_h
