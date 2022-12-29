#include "screenshot.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdio.h>

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


/* Get pixel from gixen bitmap at (x, y) */
static pixel_t *
pixel_at(bitmap_t *bitmap, int x, int y)
{
  return bitmap->pixels + bitmap->width * y + x;
};


/* X11 error handling */
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
  /* Get display and root window */
  Display *display = XOpenDisplay(NULL);

  /* set error handler */
  XSetErrorHandler(XHandleError);

  /* XOpenDisplay may set errno to 11
   * even though display is not NULL
   */
  if (!display)
  {
    fprintf(stderr, "Error opening display: %s", strerror(errno));
    return 1;
  }

  Window root = DefaultRootWindow(display);

  /* Get windows attrs */
  XWindowAttributes gwa;
  Status s = XGetWindowAttributes(display, root, &gwa);

  /* Getting image of root window */
  XImage *image = XGetImage(display,
			    root,
			    0, 0,
			    gwa.width, gwa.height,
			    AllPlanes,
			    ZPixmap);

  /* If there is error during getting an image */
  if (image == NULL)
  {
    fprintf(stderr, "Error getting screen image");
    XCloseDisplay(display);
    display = NULL;
    return 1;
  }

  /* Creating bitmap for screenshot */
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
      pixel_at(&screenshot, y, x)->red = pixel & (image->red_mask) >> 16;
    }
  }

  /* Start creating png image */
  FILE *fp = NULL;
  png_structp pngp = NULL;
  png_infop png_infop = NULL;
  png_bytepp row_pointers = NULL;

  int pixel_size = 3;
  /* aka sample in spec */
  int depth = 8;

  /*
   * Path validation
   * if given path with out a backslash,
   * append it (ignoring .png file itself)
   *
   */
  int init_path_len = strlen(path);
  if (strlen(path) != 1 &&
      path[init_path_len - 1] != '/' &&
      strcmp(path + init_path_len - 4, ".png") != 0) strcat(path, "/");

  /* add timestamp */
  time_t current_time = time(NULL);
  if (current_time == -1)
  {
      
  }

  char timestamp[64];
  strftime(timestamp,
	   sizeof(timestamp),
	   "%Y-%m-%d %H:%M:%S",
	   localtime(&current_time));

  /* if didn't get time */
  if (errno != 0)
  {
    fprintf(stderr, "Error getting the timestamp: %s", strerror(errno));
    XCloseDisplay(display);
    display = NULL;
    free(screenshot.pixels);
    screenshot.pixels = NULL;
    return 1;
  }

  
  if (strcmp(path + init_path_len - 4, ".png") != 0 && strcmp(path, ".") == 0)
  {
    path = "screenshot-";
    strcat(timestamp, ".png");
    strcat(path, timestamp);
  }
  else
  {
    if (strcmp(path + init_path_len - 4, ".png") != 0)
    {
      char *postfix = "screenshot-";
      strcat(timestamp, ".png");
      strcat(postfix, timestamp);
      strcat(path, postfix);
    }
  }

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
