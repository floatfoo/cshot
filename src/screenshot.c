#include "screenshot.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <errno.h>
#include <png.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "path.h"

/* X11 error handling */
int XHandleError(Display *display, XErrorEvent *e) {
  int buf_len = 150;
  char buf[buf_len];
  XGetErrorText(display, e->error_code, buf, buf_len);
  fprintf(stderr, "%s\n", buf);
  return 0;
};

/* get bitmap from x11 api */
bitmap_t *x_get_bitmap(int *status) {
  bitmap_t *screenshot = (bitmap_t *)malloc(sizeof(bitmap_t));

  /* Get display and root window */
  Display *display = XOpenDisplay(NULL);

  /* set error handler */
  XSetErrorHandler(XHandleError);

  /* XOpenDisplay may set errno to 11
   * even though display is not NULL
   */
  if (!display) {
    *status = ERRDISPLAY;
    screenshot = NULL;
    goto display;
  }

  Window root = DefaultRootWindow(display);

  /* Get windows attrs */
  XWindowAttributes gwa;
  XGetWindowAttributes(display, root, &gwa);

  /* Getting image of root window */
  XImage *image =
      XGetImage(display, root, 0, 0, gwa.width, gwa.height, AllPlanes, ZPixmap);

  /* If there is error during getting an image */
  if (image == NULL) {
    *status = ERRIMG;
    screenshot = NULL;
    goto display;
  }

  /* Creating bitmap for screenshot */
  screenshot->height = gwa.height;
  screenshot->width = gwa.width;
  /* bitmap allocating */
  screenshot->pixels =
      calloc(screenshot->width * screenshot->height, sizeof(pixel_t));

  /* fill the bitmap image */
  for (size_t y = 0; y < screenshot->width; ++y) {
    for (size_t x = 0; x < screenshot->height; ++x) {
      uint32_t pixel = image->f.get_pixel(image, y, x);
      pixel_at(screenshot, y, x)->green = (pixel & image->green_mask) >> 8;
      pixel_at(screenshot, y, x)->red = (pixel & image->red_mask) >> 16;
      pixel_at(screenshot, y, x)->blue = pixel & image->blue_mask;
    }
  }

  image->f.destroy_image(image);
  image = NULL;

display:
  XCloseDisplay(display);
  display = NULL;
  return screenshot;
};

int take_screenshot(char *path, bitmap_t *(get_bitmap)(int *)) {
  int status = 0;

  bitmap_t *screenshot = get_bitmap(&status);
  if (!screenshot) {
    if (status == ERRDISPLAY)
      perror("Error opening display");
    else if (status == ERRIMG)
      perror("Error getting screen image");

    goto bitmap;
  }

  /* Start creating png image */
  FILE *fp = NULL;
  png_structp pngp = NULL;
  png_infop png_infop = NULL;
  png_bytepp row_pointers = NULL;

  int pixel_size = 3;
  /* aka sample in spec */
  int depth = 8;

  char *path_to_image = create_path(path, &status);
  if (!path_to_image) {
    if (status == ERRFILECREATION)
      fprintf(stderr, "File already exists!");
    else if (status == ERRTIMESTAPS)
      perror("Error getting the timestamp");

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
  png_set_IHDR(pngp, png_infop, screenshot->width, screenshot->height, depth,
               PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  /* pointers to row
   * each row contain bytes
   * describing pixel
   */
  row_pointers = png_malloc(pngp, screenshot->height * sizeof(png_byte *));
  for (size_t y = 0; y < screenshot->height; ++y) {
    png_bytep row =
        png_malloc(pngp, sizeof(uint8_t) * screenshot->width * pixel_size);
    row_pointers[y] = row;
    for (size_t x = 0; x < screenshot->width; ++x) {
      pixel_t *pixel = pixel_at(screenshot, x, y);
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
  free((char *)path_to_image);

bitmap:
  free((pixel_t *)screenshot->pixels);
  free((bitmap_t *)screenshot);
  return status;
};
