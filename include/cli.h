#ifndef TXTURE2_CLI_H
#define TXTURE2_CLI_H

#include "image.h"
#include "writer.h"

#define default_height 600
#define default_width 600
#define default_max_colors 255
#define default_base_filename "out"
#define default_gentype simplex

struct Settings {
	ImageProps	  image_props;
	const char*	  base_filename;
	GeneratorType gentype;
};

void
parse_args (struct Settings* out, int argc, char* argv[]);

#endif // !TXTURE2_CLI_H
