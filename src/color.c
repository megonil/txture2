#include "image.h"
#include "utils.h"

#include <color.h>
#include <omp.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

extern ColoringType coloring_type;

static inline void
apply_to_color_rgb (Color* c, Color v)
{
	c->r = v.r;
	c->g = v.g;
	c->b = v.b;
}

static inline void
apply_to_color_mono (Color* c, pix v)
{ c->r = c->g = c->b = v; }

void
apply_to_color (Color* c, Color v)
{
	switch (coloring_type) {
		case Mono: apply_to_color_mono (c, v.r); break;
		case Rgb: apply_to_color_rgb (c, v); break;
	}
}

void
colors_init (Colors* c, uint width, uint height)
{
	*c = malloc (sizeof (Color*) * height);

	for (pxpos y = 0; y < height; ++y) {
		(*c)[y] = malloc (sizeof (Color) * width);
	}
}

bool
colors_for (Colors c, ImageProps p, ForAllPixelsArgs thr, void* s)
{
	atomic_bool cancel = false;
#pragma omp parallel shared(cancel)
	{
		void* thread_state = 0;

		if (thr.prep_thr) thread_state = thr.prep_thr ();

#pragma omp for collapse(2)
		for (pxpos y = 0; y < p.height; ++y) {
			for (pxpos x = 0; x < p.width; ++x) {
				ForPixelArgs args
					= {.x		  = x,
					   .y		  = y,
					   .props	  = p,
					   .gen_state = s,
					   .result	  = &c[y][x],
					   .thr_state = thread_state,
					   .cancel	  = &cancel};

				thr.f (&args);
			}
		}

		if (thread_state) thr.clear_thr (thread_state);
	}

	return atomic_load (&cancel);
}

void
colors_free (Colors c, uint height)
{
	for (uint y = 0; y < height; ++y) { free (c[y]); }

	free (c);
}

pix
to_pixsf (float x, uint16_t max_colors)
{
	// translate to [0..1]
	x += 1.0f;
	x /= 2.0f;

	// expand
	return (pix) (x * max_colors);
}
