#include "cli.h"

#include "globals.h"
#include "utils.h"

#include <cargs.h>
#include <writer.h>

static void
default_settings (struct Settings* s)
{
	s->base_filename = default_base_filename;
	s->gentype		 = default_gentype;
	s->image_props	 = (ImageProps){
		.height		= default_height,
		.width		= default_width,
		.max_colors = default_max_colors};
}

static const struct cag_option options[] = {
	{.identifier	 = 'w',
	 .access_letters = "wW",
	 .access_name	 = "width",
	 .value_name	 = "NUM",
	 .description	 = "Width of the output image"},
	{.identifier	 = 'h',
	 .access_letters = "hH",
	 .access_name	 = "height",
	 .value_name	 = "NUM",
	 .description	 = "Height of the output image"},
	{.identifier	 = 'm',
	 .access_letters = "mM",
	 .access_name	 = "max",
	 .value_name	 = "NUM",
	 .description	 = "Maximum value for each RGB component"},
	{.identifier	 = 'e',
	 .access_letters = NULL,
	 .access_name	 = "help",
	 .description	 = "Shows the command help"},
	{.identifier	 = 's',
	 .access_letters = "sS",
	 .access_name	 = "save",
	 .value_name	 = "STR",
	 .description	 = "Specifies filename for the output image"},
	{.identifier	 = 'f',
	 .access_letters = "fF",
	 .access_name	 = "freq",
	 .value_name	 = "FLT",
	 .description	 = "Specifies frequency to use with generator"},
};

static float
parse (const char* s, const char* m)
{
	int			ok;
	const float i = parse_float (s, &ok);
	if (!ok || i < 0.0f) { error ("wrong option argument %s", m); }

	return i;
}

static inline float
parse_flt (const char* s)
{ return parse (s, "float number"); }

static uint
parse_uint (const char* s)
{
	const float x = parse (s, "integer");
	if (x < 0) error ("expected a positive number");
	if (x - (int) x != 0.0) warn ("ignoring fractional part");

	return (uint) x;
}

static _Noreturn void
usage (cag_option_context* ctx)
{
	printf (
		"Usage: txture [GENERATOR] [OPTION]...\n"
		"       Generate a texture using [generator](simplex|txr-lang "
		"script)\n\n");
	cag_option_print (options, CAG_ARRAY_SIZE (options), stdout);
	exit (0);
}

#define option_v(c, v, t)                                                 \
	case c:                                                               \
		value = cag_option_get_value (&ctx);                              \
		v	  = parse_##t (value);                                        \
		break

#define option_uint(c, v) option_v (c, v, uint)
#define option_float(c, v) option_v (c, v, flt)

void
parse_args (struct Settings* out, int argc, char* argv[])
{
	default_settings (out);

	// variable to use in getting cli options arguments
	const char* value = 0;

	cag_option_context ctx;
	cag_option_init (&ctx, options, CAG_ARRAY_SIZE (options), argc, argv);

	while (cag_option_fetch (&ctx)) {
		// clang-format off
		switch (cag_option_get_identifier (&ctx)) {
		option_uint ('w', out->image_props.width);
		option_uint ('h', out->image_props.height);
		option_uint ('m', out->image_props.max_colors);
		option_float ('f', osc_freq);

		case 's': out->base_filename = cag_option_get_value (&ctx); break;
		case 'e': usage (&ctx); break;
		case '?': cag_option_print_error (&ctx, stderr); break;
		}
		// clang-format on
	}

	// generator is position-independent
	uint param_index = cag_option_get_index (&ctx);
	if (param_index >= argc) {
		note (
			"using default `%s` generator", gen_to_str (default_gentype));
	} else {
		const char* generator_str = argv[param_index];
		out->gentype			  = writer_from_string (generator_str);
	}
}

#undef option_uint
#undef option_float
#undef option_v
