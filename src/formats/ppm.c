#include <formats/ppm.h>
#include <stdio.h>
#include <utils.h>

static void
write_header (FILE* f, ImageProps props)
{
	fprintf (
		f,
		"%s %u %u %u\n",
		ppm_magic,
		props.width,
		props.height,
		props.max_colors);
}

static void
write_pixels (FILE* f, Color** pixels, ImageProps props)
{
	uint bytes_for_color = props.max_colors >= 256 ? 2 : 1;
	for (uint y = 0; y < props.height; ++y) {
		for (uint x = 0; x < props.width; ++x) {
			Color c = pixels[y][x];
			fwrite (&c.r, bytes_for_color, 1, f);
			fwrite (&c.g, bytes_for_color, 1, f);
			fwrite (&c.b, bytes_for_color, 1, f);
		}
	}
}

void
ppm_image (const char* filename, Color** pixels, ImageProps props)
{
	FILE* file = fopen (filename, "w+b");

	write_header (file, props);
	write_pixels (file, pixels, props);

	fclose (file);
}
