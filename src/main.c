#include "color.h"
#include "formats/ppm.h"
#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main (int argc, char* argv[])
{
	uint width = 400, height = 600;

	const char* filename = "out.ppm";
	Color**		colors	 = malloc (sizeof (Color*) * height);

	for (uint i = 0; i < height; ++i)
		{
			colors[i] = malloc (sizeof (Color) * width);

			for (uint j = 0; j < width; ++j)
				{
#define c colors[i][j]
					c.r = 100;
					c.g = 250;
					c.b = 50;
				}
		}

	ImageProps props;
	props.height	 = height;
	props.width		 = width;
	props.max_colors = 255;

	ppm_write (filename, colors, props);

	free (colors);

	return 0;
}
