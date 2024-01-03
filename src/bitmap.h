#ifndef BITMAP_H

#include <stdint.h>
#include <stdio.h>
#define BITMAP_H

typedef struct _pixel_t
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel_t;

typedef struct _bitmap_t
{
    pixel_t *pixels;
    size_t width, height;
} bitmap_t;

/* Get pixel from gixen bitmap at (x, y) */
pixel_t *pixel_at (bitmap_t *bitmap, int x, int y);

#endif
