#ifndef TXTURE2_TXR_VMBUILTIN_H
#define TXTURE2_TXR_VMBUILTIN_H

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

#define h() props->height
#define w() props->width
#define m() props->max_colors

#define EXTERNAL_BUILTIN_VARIABLES                                        \
	EVAR (X_VARIABLE, x)                                                  \
	EVAR (Y_VARIABLE, y)                                                  \
	EVAR (W_VARIABLE, w ())                                               \
	EVAR (H_VARIABLE, h ())                                               \
	EVAR (U_VARIABLE, x / (w () - 1))                                     \
	EVAR (V_VARIABLE, y / (h () - 1))                                     \
	EVAR (M_VARIABLE, m ())

#define BUILTIN_VARIABLES                                                 \
	VAR (R_VARIABLE, 0.0)                                                 \
	VAR (G_VARIABLE, 0.0)                                                 \
	VAR (B_VARIABLE, 0.0)

#endif // !TXTURE2_TXR_VMBUILTIN_H
