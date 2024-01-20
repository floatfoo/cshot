#include "../src/bitmap.h"
#include "../src/path.h"
#include <stdio.h>
#include <string.h>

int
test_path_maybe_append_baskslash()
{
  if (strcmp("/home/", path_maybe_append_baskslash("/home"))) {
    return 0;
  }

  if (strcmp("/home/", path_maybe_append_baskslash("/home/"))) {
    return 0;
  }

  if (strcmp("/home/user/", path_maybe_append_baskslash("/home/user"))) {
    return 0;
  }

  if (strcmp("/home/user/", path_maybe_append_baskslash("/home/user/"))) {
    return 0;
  }
  return 1;
}

int
test_path_maybe_append_baskslash_n()
{
  if (strcmp("/home/",
             path_maybe_append_baskslash_n("/home", strlen("/home")))) {
    return 0;
  }

  if (strcmp("/home/",
             path_maybe_append_baskslash_n("/home/", strlen("/home/")))) {
    return 0;
  }

  if (strcmp(
        "/home/user/",
        path_maybe_append_baskslash_n("/home/user", strlen("/home/user")))) {
    return 0;
  }

  if (strcmp(
        "/home/user/",
        path_maybe_append_baskslash_n("/home/user/", strlen("/home/user/")))) {
    return 0;
  }
  return 1;
}

int
test_create_path()
{
  int status = 0;
  printf("{%s}", create_path("/home/floatfoo/", 0));
  return status == 0;
}

int
main()
{
  puts("TEST RUN");
  int32_t testing_result = 1;
  int32_t testing_result_count = 1;

  puts("TEST test_path_maybe_append_baskslash:");
  ++testing_result_count;
  if (test_path_maybe_append_baskslash()) {
    puts("PASSED");
    ++testing_result;
  } else {
    puts("FAILED");
  }

  puts("TEST test_path_maybe_append_baskslash_n:");
  ++testing_result_count;
  if (test_path_maybe_append_baskslash_n()) {
    puts("PASSED");
    ++testing_result;
  } else {
    puts("FAILED");
  }

  puts("TEST test_create_path:");
  ++testing_result_count;
  if (test_create_path()) {
    puts("PASSED");
	++testing_result;
  } else {
	  puts("FAILED");
  }

  if (testing_result == testing_result_count) {
    puts("RESULT: PASSED");
    return 0;
  } else {
    puts("RESULT: FAILED");
    return 1;
  }
}
