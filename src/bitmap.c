#include "bitmap.h"

pixel_t *pixel_at(bitmap_t *bitmap, int x, int y) {
  return bitmap->pixels + bitmap->width * y + x;
};
