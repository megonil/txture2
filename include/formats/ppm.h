#ifndef TXTURE2_PPM_FORMAT_H
#define TXTURE2_PPM_FORMAT_H

#include "color.h"
#include "image.h"

#define ppm_magic "P6"
#define ppm_filename ".ppm"

/// Write to PPM file pixels
void
ppm_write (const char* filename, Color** pixels, ImageProps props);

#endif // !TXTURE2_PPM_FORMAT_H
