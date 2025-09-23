#include "args_handler.h"

#include <stdio.h>
#include <string.h>

// Function to handle arguments in format -opt optionValue
// argc and argv are the main function arguments
// opt_name is the option name (without the '-'), for example: "opt"
// Returns pointer to the option value, or NULL if not found
char *get_option_value(int argc, char *argv[], char *opt_name) {
  char opt_format[64];
  snprintf(opt_format, sizeof(opt_format), "-%s", opt_name);

  for (int i = 1; i < argc - 1; ++i) {
    if (strcmp(argv[i], opt_format) == 0) {
      return argv[i + 1];
    }
  }
  return NULL;
}
