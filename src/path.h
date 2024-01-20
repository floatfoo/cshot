#ifndef PATH_H
#define PATH_H

#include <stdint.h>
#include <stddef.h>

/**
 * Pure path header
 */

/**
 * Validate and create unix path for image file.
 */
char*
create_path(char* path, int* status);

/**
 * Append slash filesystem path delimitor if:
 * - given path is not alread /-ended
 * - given path is zero or one characters long
 * Note: use null-terminated string
 * Error: set's errno on error (memory allocation done inside)
 * Returns char*, which needs to be freed by client
 */
char* 
path_maybe_append_baskslash(const char* path);

/**
 * Same as maybe_append_baskslash, but allow non null-terminated paths,
 * since you must provide size n by yourself.
 */
char*
path_maybe_append_baskslash_n(const char* path, size_t n);

#endif
