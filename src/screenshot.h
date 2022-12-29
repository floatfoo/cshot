#ifndef SCREENSHOT_H

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <string.h>
#include <png.h>
#include <stdint.h>

#define SCREENSHOT_H


int take_screenshot(char *path);

#endif
