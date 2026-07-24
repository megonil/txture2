#ifndef TXTURE2_OSC_SIMPLEX
#define TXTURE2_OSC_SIMPLEX

#include "utils.h"

#include <stdint.h>

struct osn_context;

int
simplex_seed (int64_t seed, struct osn_context** ctx);

void
simplex_free (struct osn_context* ctx);

float
simplex_noise (struct osn_context* ctx, pxpos x, pxpos y, float freq);

#endif // !TXTURE2_OSC_SIMPLEX
