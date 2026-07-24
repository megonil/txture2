#include "image.h"
#include "utils.h"

#include <color.h>
#include <stdint.h>
#include <stdlib.h>

void
apply_to_color (Color* c, pix v)
{
	c->r = c->g = c->b = v;
}

void
colors_init (Colors* c, uint width, uint height)
{
	*c = malloc (sizeof (Color*) * height);

	for (pxpos y = 0; y < height; ++y)
		{
			(*c)[y] = malloc (sizeof (Color) * width);
		}
}

void
colors_for (Colors c, ImageProps p, forpixel f, void* s)
{
	for (pxpos y = 0; y < p.height; ++y)
		{
			for (pxpos x = 0; x < p.width; ++x)
				{
					apply_to_color (&c[y][x], f (x, y, p, s));
				}
		}
}

void
colors_free (Colors c, uint height)
{
	for (uint y = 0; y < height; ++y)
		{
			free (c[y]);
		}

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
