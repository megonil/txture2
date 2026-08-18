#include "txr/vmbuiltin.h"

#include "array.h"

#include <math.h>

tvalue
txr_builtin_sin (tvalue* args)
{ return sin (head (args)); }
