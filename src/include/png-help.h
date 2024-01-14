#ifndef __PNG_HELP__
#define __PNG_HELP__

#include "ff.h"

void display_init(void);
void display_clear(uint16_t);
void display_png(FIL *);
void display_png_at(FIL *, uint16_t, uint16_t);

#endif