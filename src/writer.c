#include "image.h"
#include "txr/chunk.h"
#include "txr/parser.h"
#include "txr/vm.h"
#include "utils.h"

#include <bits/pthreadtypes.h>
#include <color.h>
#include <omp.h>
#include <osc/simplex.h>
#include <stdint.h>
#include <writer.h>

extern double		osc_freq;
extern int64_t		seed;
extern ColoringType coloring_type;

static const char* txr_script_path = 0;

typedef void* (*preparefn) (void);
typedef void (*clearfn) (void*);

#define O(Name, prep, prep_thr, osc, clr, clr_thr)                        \
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

void
simplexf (void* arg)
{
	ForPixelArgs* args = arg;
	pix			  v	   = to_pixsf (
		simplex_noise (args->gen_state, args->x, args->y, osc_freq),
		args->props.max_colors);

	// in mono only R value is used
	apply_to_color (args->result, (Color){.r = v});
}

void
simplex_clear (void* ctx)
{ simplex_free (ctx); }

typedef struct {
	preparefn		 prepare;
	prepare_threadfn prepare_thread;
	forpixelfn		 genf;
	clearfn			 clear, clear_thread;
} GeneratorProps;

typedef struct {
	char*  source;
	Chunk* chunk;
} TxrGenState;

typedef struct {
	VM* vm;
} TxrThreadState;

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

	return state;
}

static void*
txr_prepare_thread ()
{
	TxrThreadState* thr_state = malloc (sizeof (TxrThreadState));
	VM*				vm		  = make_vm ();
	thr_state->vm			  = vm;

	return thr_state;
}

static void
txr_gen (void* arg)
{
	ForPixelArgs*	args	  = arg;
	TxrGenState*	gen_s	  = (TxrGenState*) args->gen_state;
	TxrThreadState* thr_state = (TxrThreadState*) args->thr_state;
	VMResult		r		  = execute (
		thr_state->vm, gen_s->chunk, args->x, args->y, &args->props, 1);

	if (r.code.kind != VMOK) {
		vm_print (r);
		exit (1);
	}

	apply_to_color (args->result, r.color);
}

static void
txr_clear_thread (void* state)
{
	TxrThreadState* s = (TxrThreadState*) state;
	vm_free (s->vm);
	free (s);
}

static void
txr_clear (void* state)
{
	TxrGenState* gen_s = (TxrGenState*) state;
	chunk_free (gen_s->chunk);
	free (gen_s->source);
	free (gen_s);
}

#define O(Name, prep, prep_thr, osc, clr, clr_thr)                        \
	{.prepare		 = prep,                                              \
	 .prepare_thread = prep_thr,                                          \
	 .genf			 = osc,                                               \
	 .clear			 = clr,                                               \
	 .clear_thread	 = clr_thr},

// clang-format off
static const GeneratorProps gens[] = {
	OSCS // ,
	O(TXRScript, txr_prepare, txr_prepare_thread, txr_gen, txr_clear, txr_clear_thread)
};
// clang-format off

#undef O

#define gen(g) gens[g]

void
write_colors (Colors c, GeneratorType t, ImageProps props)
{
	GeneratorProps gen = gen (t);

	void* state = gen.prepare ();
	colors_for (c, props, (ForAllPixelsArgs){.f = gen.genf, .prep_thr = gen.prepare_thread, .clear_thr = gen.clear_thread}, state);
	gen.clear (state);
}

#undef gen
