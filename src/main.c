#include "writer.h"

#include <color.h>
#include <formats/ppm.h>
#include <osc/simplex.h>
#include <stdint.h>
#include <time.h>
#include <utils.h>

float	simplex_freq = 1.0f;
int64_t seed;

int
main (int argc, char* argv[])
{
	uint width = 400, height = 600, max_colors = 255;

	seed = time (0);

	ImageProps props;
	props.height	 = height;
	props.width		 = width;
	props.max_colors = max_colors;

	const char* filename = "out" ppm_ext;
	Colors		colors;
	colors_init (&colors, width, height);

	write_colors (colors, Simplex, props);

	ppm_image (filename, colors, props);

	colors_free (colors, height);
	return 0;
}
