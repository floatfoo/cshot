#ifndef SCREENSHOT_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <string.h>
#include <png.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>

#define SCREENSHOT_H

/* define some error codes */
#define ERRDISPLAY 1
#define ERRIMG 2
#define ERRTIMESTAPS 3
#define ERRFILECREATION 4
#define ERRPNG 5
#define ERRPNGINFO 6

int take_screenshot(char *path);

#endif
