#include "lib/args_handler/args_handler.h"
#include "lib/file_reader/file_reader.h"
#include "lib/geo_handler/geo_handler.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

  if (argc > 10) { // program -e path -f .geo -o output -q .qry suffix
    printf("Error: Too many arguments\n");
    exit(1);
  }

  // Get arguments
  const char *output_path = get_option_value(argc, argv, "o");
  const char *geo_input_path = get_option_value(argc, argv, "f");
  const char *qry_input_path = get_option_value(argc, argv, "q");
  const char *command_suffix = get_command_suffix(argc, argv);

  // Verify required arguments
  if (geo_input_path == NULL || output_path == NULL) {
    printf("Error: -f and -o are required\n");
    exit(1);
  }

  FileData geo_file = file_data_create(geo_input_path);
  if (geo_file == NULL) {
    printf("Error: Failed to create FileData\n");
    exit(1);
  }

 Ground ground = execute_geo_commands(geo_file, output_path, command_suffix);

  file_data_destroy(geo_file);

  destroy_geo_waste(ground);

  return 0;
}