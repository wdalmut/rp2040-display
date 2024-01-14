#ifndef __PNG_HELP__
#define __PNG_HELP__

#include "ff.h"

typedef struct {
    FIL *file;
} custom_file;

void display_init(void);
void display_png(FIL *);
#endif