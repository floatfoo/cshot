#include "screenshot.h"
#include <argp.h>
#include <stdio.h>

#define ARGPC 1

const char *argp_program_version = "1.0";
const char *argp_program_bug_address = "1.0";
static char doc[] = "cshot - simple x11 screenshot facility";

static struct argp_option options[] = {
    {"path", 'p', "path", 0, "Save screenshot to provided path", -1},
};

struct arguments {
  char *args[ARGPC];
  char *output_path;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;

  switch (key) {
  case 'p':
    arguments->output_path = arg;
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }

  return 0;
};

static struct argp argp = {options, parse_opt, 0, doc, NULL, NULL, NULL};

int main(int argc, char **argv) {
  struct arguments arguments;
  arguments.output_path = "~";

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  return take_screenshot(arguments.output_path, x_get_bitmap) == 0 ? 0 : 1;
};
