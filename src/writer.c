#include "image.h"
#include "utils.h"

#include <color.h>
#include <osc/simplex.h>
#include <stdint.h>
#include <writer.h>

extern float   simplex_freq;
extern int64_t seed;

typedef void* (*preparefn) ();
typedef pix (*genfn) (pxpos x, pxpos y, ImageProps props, void*);
typedef void (*clearfn) (void*);

void*
simplex_prepare ()
{
	struct osn_context* ctx;
	simplex_seed (seed, &ctx);
	return ctx;
}

#define O(Name, prep, osc, clr)                                           \
	if (streq (s, #Name)) {                                               \
		return Name;                                                      \
	}

GeneratorType
writer_from_string (const char* s)
{
	OSCS;
	// else
	error ("unknown generator: %s", s);
}

#undef O

pix
simplexf (pxpos x, pxpos y, ImageProps props, void* ctx)
{
	return to_pixsf (
		simplex_noise (ctx, x, y, simplex_freq), props.max_colors);
}

void
simplex_clear (void* ctx)
{
	simplex_free (ctx);
}

typedef struct {
	preparefn prepare;
	genfn	  genf;
	clearfn	  clear;
} GeneratorProps;

#define O(Name, prep, osc, clr)                                           \
	{.prepare = prep, .genf = osc, .clear = clr},

static const GeneratorProps gens[] = {OSCS};

#undef O

#define gen(g) gens[g]

void
write_colors (Colors c, GeneratorType t, ImageProps props)
{
	GeneratorProps gen = gen (t);

	void* state = gen.prepare ();
	colors_for (c, props, gen.genf, state);
	gen.clear (state);
}

#undef gen
