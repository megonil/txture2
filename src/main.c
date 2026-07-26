#include "cli.h"
#include "formats/ppm.h"
#include "image.h"
#include "writer.h"

#include <assert.h>
#include <color.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

float	simplex_freq = 1.0f;
int64_t seed;

static char*
concat_filename (const char* base, const char* extension)
{
	assert (strlen (extension) == 4);
	size_t base_len = strlen (base);

	// 4 for the extension and 1 for nul-terminal
	char* b = malloc (base_len + 5);
	memcpy (b, base, base_len);
	memcpy (b + base_len, extension, 4);
	b[base_len + 4] = '\0';

	return b;
}

static inline void
initialize_seed ()
{ seed = time (0); }

int
main (int argc, char* argv[])
{
	initialize_seed ();

	struct Settings settings;
	parse_args (&settings, argc, argv);

	ImageProps image = settings.image_props;

	char* out_filename = concat_filename (settings.base_filename, ppm_ext);

	Colors colors;
	colors_init (&colors, image.width, image.height);

	write_colors (colors, settings.gentype, image);
	ppm_image (out_filename, colors, image);

	free (out_filename);
	colors_free (colors, image.height);
	return 0;
}
