#ifndef SCREENSHOT_H

#define SCREENSHOT_H

/* define some error codes */
#define ERRDISPLAY 1
#define ERRIMG 2
#define ERRTIMESTAPS 3
#define ERRFILECREATION 4
#define ERRPNG 5
#define ERRPNGINFO 6

typedef struct bitmap_t bitmap_t;

bitmap_t *x_get_bitmap(int *status);
char *create_unix_path(char *path, int *status);

int take_screenshot(char *path, bitmap_t *(get_bitmap)(int *));

#endif
