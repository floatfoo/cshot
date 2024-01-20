#include "screenshot.h"

#include "bitmap.h"
#include "display_server.h"
#include "path.h"
#include <errno.h>
#include <png.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
take_screenshot(char* path, bitmap_t*(get_bitmap)(int*))
{
  int status = 0;

  bitmap_t* screenshot = get_bitmap(&status);
  if (!screenshot) {
    if (status == ERRDISPLAY)
      perror("Error opening display");
    else if (status == ERRIMG)
      perror("Error getting screen image");

    goto bitmap;
  }

  /* Start creating png image */
  FILE* fp = NULL;
  png_structp pngp = NULL;
  png_infop png_infop = NULL;
  png_bytepp row_pointers = NULL;

  int pixel_size = 3;
  /* aka sample in spec */
  int depth = 8;

  char* path_to_image = create_path(path, &status);
  if (!path_to_image) {
    if (status == ERRFILECREATION)
      fprintf(stderr, "File already exists!\n");
    else if (status == ERRTIMESTAPS)
      perror("Error getting the timestamp");
    else if (status == ERRMEMALLOC)
      perror("Error getting the memory for the path");

    goto path;
  }

  /* file creation */
  fp = fopen(path_to_image, "wb");

  if (!fp) {
    perror("Error creating the file");
    status = ERRFILECREATION;
    goto bitmap;
  }

  /* write png creation */
  pngp = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!pngp) {
    perror("Error creating png struct");
    status = ERRPNG;
    goto file;
  }

  png_infop = png_create_info_struct(pngp);
  if (!png_infop) {
    perror("Error creating png info struct");
    status = ERRPNGINFO;
    goto png;
  }

  /* something went wrong with png struct creation */
  /* aka setup error handling */
  if (setjmp(png_jmpbuf(pngp))) {
    perror("Error creating png struct");
    status = ERRPNG;
    goto png;
  }

  /* set IHDR png header */
  png_set_IHDR(pngp,
               png_infop,
               screenshot->width,
               screenshot->height,
               depth,
               PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  /* pointers to row
   * each row contain bytes
   * describing pixel
   */
  row_pointers = png_malloc(pngp, screenshot->height * sizeof(png_byte*));
  for (size_t y = 0; y < screenshot->height; ++y) {
    png_bytep row =
      png_malloc(pngp, sizeof(uint8_t) * screenshot->width * pixel_size);
    row_pointers[y] = row;
    for (size_t x = 0; x < screenshot->width; ++x) {
      pixel_t* pixel = pixel_at(screenshot, x, y);
      *row++ = pixel->red;
      *row++ = pixel->green;
      *row++ = pixel->blue;
    }
  }

  /* init I/O to the FILE */
  png_init_io(pngp, fp);
  png_set_rows(pngp, png_infop, row_pointers);
  png_write_png(pngp, png_infop, PNG_TRANSFORM_IDENTITY, NULL);

  for (size_t y = 0; y < screenshot->height; y++) {
    png_free(pngp, row_pointers[y]);
    row_pointers[y] = NULL;
  }
  png_free(pngp, row_pointers);
  row_pointers = NULL;

png:
  png_destroy_write_struct(&pngp, &png_infop);
  pngp = NULL, png_infop = NULL;

file:
  fclose(fp);
  fp = NULL;

path:
  free((char*)path_to_image);

bitmap:
  free((pixel_t*)screenshot->pixels);
  free((bitmap_t*)screenshot);
  return status;
};
