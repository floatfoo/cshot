#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "path.h"
#include"screenshot.h"

char *create_path(char *path, int *status) {
  /*
   * Path validation
   * if given path with out a backslash,
   * append it (ignoring .png file itself)
   *
   */
  int init_path_len = strlen(path);
  if (strlen(path) != 1 && path[init_path_len - 1] != '/' &&
      strcmp(path + init_path_len - 4, ".png") != 0)
    strcat(path, "/");

  /* check if file already exist */
  if (strcmp(path, "~") != 0) {
    if (access(path, F_OK) == 0) {
      *status = ERRFILECREATION;
      return NULL;
    }
  }

  char *path_to_image = calloc(512, sizeof(char));
  if (!path_to_image) {
      *status = ERRMEMALLOC;
      free(path_to_image);
      return NULL;
  }

  if (strcmp(path + init_path_len - 4, ".png") != 0) {
    /* add timestamp */
    time_t current_time = time(NULL);

    char timestamp[256];
    if (strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S",
                 localtime(&current_time)) == 0) {
      *status = ERRTIMESTAPS;
      return NULL;
    }

    if (strcmp(path, ".") == 0) {
      strcat(path_to_image, "screenshot-");
      strcat(timestamp, ".png");
      strcat(path_to_image, timestamp);
    } else if (strcmp(path, "~") == 0) {
	char *home = getenv("HOME");
	char path[128];
	strcat(path, home);
	strcat(path, "/screenshot-");
	strcat(path_to_image, path);
	strcat(timestamp, ".png");
	strcat(path_to_image, timestamp);
    } else {
      char postfix[128] = "screenshot-";
      strcat(timestamp, ".png");
      strcat(postfix, timestamp);
      strcat(path_to_image, postfix);
    }
  }
  return path_to_image;
};
