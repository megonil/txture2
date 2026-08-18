#ifndef TXTURE2_TXR_VMBUILTIN_H
#define TXTURE2_TXR_VMBUILTIN_H

#include "txr/value.h"
#include "utils.h"
#define R_VARIABLE "R"
#define G_VARIABLE "G"
#define B_VARIABLE "B"

// external
#define X_VARIABLE "X"
#define Y_VARIABLE "Y"
#define W_VARIABLE "W"
#define H_VARIABLE "H"
#define U_VARIABLE "U"
#define V_VARIABLE "V"
#define M_VARIABLE "M"
#define F_VARIABLE "F"

#define BUILTIN_VARIABLES_N 11

#define h() props->height
#define w() props->width
#define m() props->max_colors
#define f() osc_freq

#define EXTERNAL_BUILTIN_VARIABLES                                        \
	EVAR (X_VARIABLE, x)                                                  \
	EVAR (Y_VARIABLE, y)                                                  \
	EVAR (W_VARIABLE, w ())                                               \
	EVAR (H_VARIABLE, h ())                                               \
	EVAR (U_VARIABLE, x / (w () - 1))                                     \
	EVAR (V_VARIABLE, y / (h () - 1))                                     \
	EVAR (M_VARIABLE, m ())                                               \
	EVAR (F_VARIABLE, f ())

#define BUILTIN_VARIABLES                                                 \
	VAR (R_VARIABLE, 0.0)                                                 \
	VAR (G_VARIABLE, 0.0)                                                 \
	VAR (B_VARIABLE, 0.0)

#define BUILTIN_FUNCTIONS B (sin, 1)

#define B(Name, Argc) tvalue txr_builtin_##Name (tvalue* args);
BUILTIN_FUNCTIONS
#undef B

typedef tvalue (*BuiltinFunctionF) (tvalue* args);

typedef struct {
	BuiltinFunctionF fun;
	uint			 argc;
} BuiltinFunction;

#endif // !TXTURE2_TXR_VMBUILTIN_H
