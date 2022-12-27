#include "screenshot.h"

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

  if (image == NULL)
    return 1;

  /* Start creating png image */
  int init_path_len = strlen(path);
  if (path[init_path_len - 1] != '/' && strcmp(path + init_path_len - 4, ".png") != 0)
    strcat(path, "/");

  printf("%s", path);

  /* Path validation */
  if (strcmp(path, ".") == 0)
    path = "screenshot.png";
  else
    strcat(path, "screenshot.png");


  /* file creation */
  FILE *fp = fopen(path, "wb");

  if (!fp) return 1;

  /* write png creation */
  png_structp pngp = png_create_write_struct(
			  PNG_LIBPNG_VER_STRING,
			  NULL,
			  NULL,
			  NULL
			  );

  png_infop png_infop = png_create_info_struct(pngp);

  /* something went wrong with png struct creation */
  if (!pngp)
    return 1;

  if (setjmp(png_jmpbuf(pngp)))
  {
    png_destroy_write_struct(&pngp, &png_infop);
    fclose(fp);
    return 1;
  }

  XCloseDisplay(display);
  return 0;
};
