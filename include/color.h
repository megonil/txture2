#ifndef TXTURE2_COLOR_H
#define TXTURE2_COLOR_H

#include "image.h"
#include "utils.h"

#include <stdint.h>

typedef uint16_t pix;
typedef struct {
	pix r, g, b;
} Color;

typedef enum { Mono, Rgb } ColoringType;

typedef Color** Colors;

void
colors_init (Colors*, uint width, uint height);

void
colors_free (Colors, uint height);

void
apply_to_color (Color*, Color);

typedef struct {
	ImageProps props;
	void*	   gen_state;
	void*	   thr_state;
	Color*	   result;
	pxpos	   x, y;
} ForPixelArgs;

typedef void (*forpixelfn) (void*);
typedef void* (*prepare_threadfn) (void);
typedef void (*clearfn) (void*);

typedef struct {
	forpixelfn		 f;
	prepare_threadfn prep_thr;
	clearfn			 clear_thr;
} ForAllPixelsArgs;

void
colors_for (Colors c, ImageProps p, ForAllPixelsArgs thr, void* s);

/// Convert Float with value in [-1..1] to pix
pix
to_pixsf (float x, uint16_t max_colors);

#endif // !TXTURE2_COLOR_H
