#include "image.h"
#include "txr/chunk.h"
#include "txr/parser.h"
#include "txr/vm.h"
#include "utils.h"

#include <color.h>
#include <osc/simplex.h>
#include <stdint.h>
#include <writer.h>

extern float		osc_freq;
extern int64_t		seed;
extern ColoringType coloring_type;

static const char* txr_script_path = 0;

typedef void* (*preparefn) ();
typedef Color (*genfn) (pxpos x, pxpos y, ImageProps props, void*);
typedef void (*clearfn) (void*);

#define O(Name, prep, osc, clr)                                           \
	if (streq (s, #Name)) {                                               \
		coloring_type = Mono;                                             \
		return Name;                                                      \
	}

GeneratorType
writer_from_string (const char* s)
{
	OSCS;
	// or
	txr_script_path = s;
	coloring_type	= Rgb;
	return TXRScript;
}

#undef O

static void*
simplex_prepare ()
{
	struct osn_context* ctx;
	simplex_seed (seed, &ctx);
	return ctx;
}

Color
simplexf (pxpos x, pxpos y, ImageProps props, void* ctx)
{
	pix v
		= to_pixsf (simplex_noise (ctx, x, y, osc_freq), props.max_colors);

	// in mono only R value is used
	return (Color){.r = v};
}

void
simplex_clear (void* ctx)
{ simplex_free (ctx); }

typedef struct {
	preparefn prepare;
	genfn	  genf;
	clearfn	  clear;
} GeneratorProps;

typedef struct {
	char*  source;
	Chunk* chunk;
	VM*	   vm;
} TxrGenState;

static void*
txr_prepare ()
{
	TxrGenState* state = malloc (sizeof (TxrGenState));
	state->source	   = readfile (txr_script_path);

	Chunk* chunk = make_chunk ();
	state->chunk = chunk;

	Parser* parser = make_parser (state->source, txr_script_path, chunk);
	parse (parser);
	parser_free (parser);

	VM* vm	  = make_vm ();
	state->vm = vm;

	return state;
}

static Color
txr_gen (pxpos x, pxpos y, ImageProps props, void* state)
{
	TxrGenState* gen_s = (TxrGenState*) state;
	VMResult	 r	   = execute (gen_s->vm, gen_s->chunk, x, y, &props);

	if (r.code.kind != VMOK) {
		vm_print (r);
		exit (1);
	}

	return r.color;
}

static void
txr_clear (void* state)
{
	TxrGenState* gen_s = (TxrGenState*) state;
	chunk_free (gen_s->chunk);
	vm_free (gen_s->vm);
	free (gen_s->source);
	free (gen_s);
}

#define O(Name, prep, osc, clr)                                           \
	{.prepare = prep, .genf = osc, .clear = clr},

// clang-format off
static const GeneratorProps gens[] = {
	OSCS // ,
	O(TXRScript, txr_prepare, txr_gen, txr_clear)
};
// clang-format off

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
