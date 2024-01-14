#ifndef __FILE_HELP__
#define __FILE_HELP__

#include <stdio.h>
//
#include "f_util.h"
#include "ff.h"
#include "pico/stdlib.h"

/**
 * Returns the number of PNGs in the requested path
 * 
 * @return uint16_t number of PNGs in the path
 */
uint16_t enumerate_pngs(char *);

size_t get_random_png_from_path(uint16_t, char *, FIL *);

#endif