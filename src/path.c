#include "path.h"
#include "screenshot.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>


char*
path_maybe_append_baskslash(const char* path)
{
  return path_maybe_append_baskslash_n(path, strlen(path));
}

char*
path_maybe_append_baskslash_n(const char* path, size_t n)
{
  char* new_path = calloc(n + 1, sizeof(char));
  strcat(new_path, path);
  if (n < 2 || path[n - 1] == '/' || path[n] == '\\') {
    return new_path;
  }
  strcat(new_path, "/");
  return new_path;
}

char*
create_path(char* path, int* status)
{
  int init_path_len = strlen(path);
  path = path_maybe_append_baskslash(path);
  
  /* check if file already exist */
  if (strcmp(path, "~") != 0) {
    if (access(path, F_OK) == 0) {
      *status = ERRFILECREATION;
      return NULL;
    }
  }

  char* path_to_image = calloc(512, sizeof(char));
  if (!path_to_image) {
    *status = ERRMEMALLOC;
    free(path_to_image);
    return NULL;
  }

  if (strcmp(path + init_path_len - 4, ".png") != 0) {
    /* add timestamp */
    time_t current_time = time(NULL);

    char timestamp[256];
    if (strftime(timestamp,
                 sizeof(timestamp),
                 "%Y-%m-%d %H:%M:%S",
                 localtime(&current_time)) == 0) {
      *status = ERRTIMESTAPS;
      return NULL;
    }
    if (strcmp(path, ".") == 0) {
      strcat(path_to_image, "screenshot-");
      strcat(timestamp, ".png");
      strcat(path_to_image, timestamp);
    } else if (strcmp(path, "~") == 0) {
      char* home = getenv("HOME");
      char created_path[128];
      strcat(created_path, path);
      strcat(created_path, home);
      strcat(created_path, "/screenshot-");
      strcat(path_to_image, created_path);
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
