#ifndef DISPLAY_SERVER_H
#define DISPLAY_SERVER_H

#include "bitmap.h"

// wayland version
bitmap_t *w_get_bitmap(int *status);

// x11 version
bitmap_t *x_get_bitmap(int *status);

#endif
