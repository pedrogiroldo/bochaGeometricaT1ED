#include "lib/args_handler/args_handler.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

  // Verify required arguments
  if (get_option_value(argc, argv, "f") == NULL ||
      get_option_value(argc, argv, "o") == NULL) {
    printf("Error: -f and -o are required\n");
    exit(1);
  }


  
  return 0;
}