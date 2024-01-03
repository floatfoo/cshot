#include "bitmap.h"
#include "display_server.h"
#include "screenshot.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdlib.h>

/* X11 error handling */
int
XHandleError (Display *display, XErrorEvent *e)
{
    int buf_len = 150;
    char buf[buf_len];
    XGetErrorText (display, e->error_code, buf, buf_len);
    fprintf (stderr, "%s\n", buf);
    return 0;
};

/* get bitmap from x11 api */
bitmap_t *
x_get_bitmap (int *status)
{
    bitmap_t *screenshot = (bitmap_t *)malloc (sizeof (bitmap_t));
    if (!screenshot)
        {
            *status = ERRMEMALLOC;
            free (screenshot);
            screenshot = NULL;
            return screenshot;
        }

    /* Get display and root window */
    Display *display = XOpenDisplay (NULL);

    /* set error handler */
    XSetErrorHandler (XHandleError);

    /* XOpenDisplay may set errno to 11
     * even though display is not NULL
     */
    if (!display)
        {
            *status = ERRDISPLAY;
            free (screenshot);
            screenshot = NULL;
            goto display;
        }

    Window root = DefaultRootWindow (display);

    /* Get windows attrs */
    XWindowAttributes gwa;
    XGetWindowAttributes (display, root, &gwa);

    /* Getting image of root window */
    XImage *image = XGetImage (display, root, 0, 0, gwa.width, gwa.height,
                               AllPlanes, ZPixmap);

    /* If there is error during getting an image */
    if (!image)
        {
            *status = ERRIMG;
            free (screenshot);
            screenshot = NULL;
            goto display;
        }

    /* Creating bitmap for screenshot */
    screenshot->height = gwa.height;
    screenshot->width = gwa.width;
    /* bitmap allocating */
    screenshot->pixels
        = calloc (screenshot->width * screenshot->height, sizeof (pixel_t));

    /* fill the bitmap image */
    for (size_t y = 0; y < screenshot->width; ++y)
        {
            for (size_t x = 0; x < screenshot->height; ++x)
                {
                    uint32_t pixel = image->f.get_pixel (image, y, x);
                    pixel_at (screenshot, y, x)->green
                        = (pixel & image->green_mask) >> 8;
                    pixel_at (screenshot, y, x)->red
                        = (pixel & image->red_mask) >> 16;
                    pixel_at (screenshot, y, x)->blue
                        = pixel & image->blue_mask;
                }
        }

    image->f.destroy_image (image);
    image = NULL;

display:
    XCloseDisplay (display);
    display = NULL;
    return screenshot;
};

bitmap_t *
w_get_bitmap (int *)
{
    return NULL;
}
