#ifndef __DISPLAY__
#define __DISPLAY__

#include <stdint.h>
#include "png-help.h"

void display_init(void);
void display_clear(uint16_t);
void display_png(Png_t *);
void display_png_at(Png_t *, uint16_t, uint16_t);

#endif