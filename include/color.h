#ifndef TXTURE2_COLOR_H
#define TXTURE2_COLOR_H

#include "image.h"
#include "utils.h"

#include <stdint.h>

typedef uint16_t pix;
typedef struct
{
	pix r, g, b;
} Color;

typedef Color** Colors;

void
colors_init (Colors*, uint width, uint height);

void
colors_free (Colors, uint height);

void
apply_to_color (Color*, pix);

typedef pix (*forpixel) (pxpos, pxpos, ImageProps, void*);

void
colors_for (Colors c, ImageProps p, forpixel f, void* s);

/// Convert Float with value in [-1..1] to pix
pix
to_pixsf (float x, uint16_t max_colors);

#endif // !TXTURE2_COLOR_H
