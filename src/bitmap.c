#include "bitmap.h"
#include <stdint.h>
#include <stdio.h>

typedef struct pixel_t {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} pixel_t;

typedef struct bitmap_t {
  pixel_t *pixels;
  size_t width, height;
} bitmap_t;

pixel_t *pixel_at(bitmap_t *bitmap, int x, int y) {
  return bitmap->pixels + bitmap->width * y + x;
};
