#ifndef BITMAP_H

#define BITMAP_H

typedef struct pixel_t pixel_t;
typedef struct bitmap_t bitmap_t;

/* Get pixel from gixen bitmap at (x, y) */
pixel_t *pixel_at(bitmap_t *bitmap, int x, int y);

#endif
