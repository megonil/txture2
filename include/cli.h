#ifndef TXTURE2_CLI_H
#define TXTURE2_CLI_H

#include "image.h"
#include "writer.h"

struct Settings {
	ImageProps	  image_props;
	const char*	  base_filename;
	float		  simplex_freq;
	GeneratorType gentype;
};

void
parse_args (struct Settings* out, int argc, char* argv[]);

#endif // !TXTURE2_CLI_H
