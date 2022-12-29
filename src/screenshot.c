#include "screenshot.h"

typedef struct pixel_t
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
}
  pixel_t;

typedef struct bitmap_t
{
  pixel_t *pixels;
  size_t width, height;
}    
  bitmap_t;

static pixel_t *
pixel_at(bitmap_t *bitmap, int x, int y)
{
  return bitmap->pixels + bitmap->width * y + x;
};

int
XHandleError(Display *display, XErrorEvent *e)    
{
  int buf_len = 150;
  char buf[buf_len];
  XGetErrorText(display, e->error_code, buf, buf_len);
  fprintf(stderr, "%s\n", buf);
  return 0;
};

int
take_screenshot(char *path)    
{
  Display *display = XOpenDisplay(NULL);
  Window root = DefaultRootWindow(display);

  XWindowAttributes gwa;

  XGetWindowAttributes(display, root, &gwa);

  XImage *image = XGetImage(display,
			    root,
			    0, 0,
			    gwa.width, gwa.height,
			    AllPlanes,
			    ZPixmap);

  bitmap_t screenshot;
  screenshot.height = gwa.height;
  screenshot.width = gwa.width;
  /* bitmap allocating */
  screenshot.pixels = calloc(screenshot.width * screenshot.height, sizeof(pixel_t));

  /* fill the bitmap image */
  for (int y = 0; y < screenshot.width; ++y)
  {
    for (int x = 0; x < screenshot.height; ++x)
    {
      uint32_t pixel = image->f.get_pixel(image, y, x);
      pixel_at(&screenshot, y, x)->blue = pixel & image->blue_mask;
      pixel_at(&screenshot, y, x)->green = (pixel & image->green_mask) >> 8;
      pixel_at(&screenshot, y, x)->red = (pixel & image->red_mask) >> 16;
    }
  }

  if (image == NULL)
    return 1;

  /* Start creating png image */
  FILE *fp = NULL;
  png_structp pngp = NULL;
  png_infop png_infop = NULL;
  png_bytepp row_pointers = NULL;

  int pixel_size = 3;
  /* aka sample in spec */
  int depth = 8;

  int init_path_len = strlen(path);
  if (strlen(path) != 1 &&
      path[init_path_len - 1] != '/' &&
      strcmp(path + init_path_len - 4, ".png") != 0) strcat(path, "/");


  /* Path validation */
  if (strcmp(path, ".") == 0)
    path = "screenshot.png";
  else
    strcat(path, "screenshot.png");

  /* file creation */
  fp = fopen(path, "wb");

  if (!fp) {
    return 1; 
  }

  /* write png creation */
  pngp = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!pngp)
  {
    png_destroy_write_struct(&pngp, &png_infop);
    fclose(fp);
    return 1;
  }

  png_infop = png_create_info_struct(pngp);
  if (!png_infop)
  {
    png_destroy_write_struct(&pngp, &png_infop);
    fclose(fp);
    return 1;
  }

  /* something went wrong with png struct creation */
  /* aka setup error handling */
  if (setjmp(png_jmpbuf(pngp)))
  {
    png_destroy_write_struct(&pngp, &png_infop);
    fclose(fp);
    return 1;
  }

  png_set_IHDR(pngp,
	       png_infop,
	       screenshot.width, screenshot.height,
	       depth,
	       PNG_COLOR_TYPE_RGB,
	       PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_DEFAULT,
	       PNG_FILTER_TYPE_DEFAULT);

  row_pointers = png_malloc(pngp, gwa.height * sizeof(png_byte*));
  for (int y = 0; y < gwa.height; ++y)
  {
    png_bytep row = png_malloc(pngp, sizeof(uint8_t) * gwa.width * pixel_size);
    row_pointers[y] = row;
    for (int x = 0; x < gwa.width; ++x)
    {
      pixel_t *pixel = pixel_at(&screenshot, x, y);
      *row++ = pixel->red;
      *row++ = pixel->green;
      *row++ = pixel->blue;
    }
  }

  /* init I/O to the FILE */
  png_init_io(pngp, fp);
  png_set_rows(pngp, png_infop, row_pointers);
  png_write_png(pngp, png_infop, PNG_TRANSFORM_IDENTITY, NULL);

  for(int y = 0; y < screenshot.height; y++)
  {
    png_free(pngp, row_pointers[y]);
    row_pointers[y] = NULL;
  }
  png_free(pngp, row_pointers);
  row_pointers = NULL;

  free(screenshot.pixels);
  screenshot.pixels = NULL;

  png_destroy_write_struct(&pngp, &png_infop);
  pngp = NULL, png_infop = NULL;
  fclose(fp);
  fp = NULL;

  image->f.destroy_image(image);
  image = NULL;
  XCloseDisplay(display);
  return 0;
};
