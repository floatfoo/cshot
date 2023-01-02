#include "screenshot.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <unistd.h>

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
  int status = 0;

  /* Get display and root window */
  Display *display = XOpenDisplay(NULL);

  /* set error handler */
  XSetErrorHandler(XHandleError);

  /* XOpenDisplay may set errno to 11
   * even though display is not NULL
   */
  if (!display)
  {
    perror("Error opening display");
    return ERRDISPLAY;
  }

  Window root = DefaultRootWindow(display);

  /* Get windows attrs */
  XWindowAttributes gwa;
  XGetWindowAttributes(display, root, &gwa);

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
    perror("Error getting screen image");
    status = ERRIMG;
    goto display;
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
      pixel_at(&screenshot, y, x)->green = (pixel & image->green_mask) >> 8;
      pixel_at(&screenshot, y, x)->red = (pixel & image->red_mask) >> 16;
      pixel_at(&screenshot, y, x)->blue = pixel & image->blue_mask;
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

  /* check if file already exist */
  if (strcmp(path, ".") != 0)
  {
    if (access(path, F_OK) == 0)
    {
      fprintf(stderr, "File already exists!");
      status = ERRFILECREATION;
      goto bitmap;
    }
  }

  char path_to_image[512];
  if (strcmp(path + init_path_len - 4, ".png") != 0)
  {
    /* add timestamp */
    time_t current_time = time(NULL);

    char timestamp[64];
    if (strftime(timestamp,
	     sizeof(timestamp),
	     "%Y-%m-%d %H:%M:%S",
	     localtime(&current_time)) == 0)
    {
      perror("Error getting the timestamp");
      status = ERRTIMESTAPS;
      goto bitmap;
    }

    if (strcmp(path, ".") == 0)
      {
	strcat(path_to_image, "screenshot-");
	strcat(timestamp, ".png");
	strcat(path_to_image, timestamp);
      }
    else
      {
	char *postfix = "screenshot-";
	strcat(timestamp, ".png");
	strcat(postfix, timestamp);
	strcat(path_to_image, postfix);
      }
  }


  /* file creation */
  fp = fopen(path_to_image, "wb");

  if (!fp)
  {
    perror("Error creating the file");
    status = ERRFILECREATION; 
    goto bitmap;
  }

  /* write png creation */
  pngp = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!pngp)
  {
    perror("Error creating png struct");
    status = ERRPNG;
    goto file;
  }

  png_infop = png_create_info_struct(pngp);
  if (!png_infop)
  {
    perror("Error creating png info struct");
    status = ERRPNGINFO;
    goto png;
  }

  /* something went wrong with png struct creation */
  /* aka setup error handling */
  if (setjmp(png_jmpbuf(pngp)))
  {
    perror("Error creating png struct");
    status = ERRPNG;
    goto png;
  }

  /* set IHDR png header */
  png_set_IHDR(pngp,
	       png_infop,
	       screenshot.width, screenshot.height,
	       depth,
	       PNG_COLOR_TYPE_RGB,
	       PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_DEFAULT,
	       PNG_FILTER_TYPE_DEFAULT);

  /* pointers to row
   * each row contain bytes
   * describing pixel
   */
  row_pointers = png_malloc(pngp, screenshot.height * sizeof(png_byte*));
  for (int y = 0; y < gwa.height; ++y)
  {
    png_bytep row = png_malloc(pngp, sizeof(uint8_t) * screenshot.width * pixel_size);
    row_pointers[y] = row;
    for (int x = 0; x < screenshot.width; ++x)
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


  image->f.destroy_image(image);
  image = NULL;

 png:
  png_destroy_write_struct(&pngp, &png_infop);
  pngp = NULL, png_infop = NULL;

 file:
  fclose(fp);
  fp = NULL;

 bitmap:
  free(screenshot.pixels);
  screenshot.pixels = NULL;

 display:
  XCloseDisplay(display);
  display = NULL;
  return status;
};
