#include "lib/args_handler/args_handler.h"
#include "lib/file_reader/file_reader.h"
#include "lib/geo_handler/geo_handler.h"
#include "lib/qry_handler/qry_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  if (argc > 10) { // program -e path -f .geo -o output -q .qry suffix
    printf("Error: Too many arguments\n");
    exit(1);
  }

  // Get arguments
  const char *output_path = get_option_value(argc, argv, "o");
  const char *geo_input_path = get_option_value(argc, argv, "f");
  const char *prefix_path = get_option_value(argc, argv, "e");
  const char *qry_input_path = get_option_value(argc, argv, "q");
  const char *command_suffix = get_command_suffix(argc, argv);

  // Apply prefix_path if it exists (only to -f and -q, not -o)
  char *full_geo_path = NULL;
  char *full_qry_path = NULL;

  if (prefix_path != NULL) {
    size_t prefix_len = strlen(prefix_path);
    int needs_slash = (prefix_len > 0 && prefix_path[prefix_len - 1] != '/');

    if (geo_input_path != NULL) {
      size_t geo_len = strlen(geo_input_path);
      full_geo_path = (char *)malloc(prefix_len + geo_len + 2);
      if (needs_slash) {
        sprintf(full_geo_path, "%s/%s", prefix_path, geo_input_path);
      } else {
        sprintf(full_geo_path, "%s%s", prefix_path, geo_input_path);
      }
      geo_input_path = full_geo_path;
    }

    if (qry_input_path != NULL) {
      size_t qry_len = strlen(qry_input_path);
      full_qry_path = (char *)malloc(prefix_len + qry_len + 2);
      if (needs_slash) {
        sprintf(full_qry_path, "%s/%s", prefix_path, qry_input_path);
      } else {
        sprintf(full_qry_path, "%s%s", prefix_path, qry_input_path);
      }
      qry_input_path = full_qry_path;
    }
  }

  
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

  // If a .qry file was provided, execute its commands on the same ground
  if (qry_input_path != NULL) {
    FileData qry_file = file_data_create(qry_input_path);
    if (qry_file == NULL) {
      printf("Error: Failed to create FileData for .qry\n");
      destroy_geo_waste(ground);
      exit(1);
    }

    Qry qry = execute_qry_commands(qry_file, geo_file, ground, output_path);
    file_data_destroy(qry_file);
    destroy_qry_waste(qry);
  }

  file_data_destroy(geo_file);
  destroy_geo_waste(ground);

  // Free allocated memory for paths
  if (full_geo_path != NULL) free(full_geo_path);
  if (full_qry_path != NULL) free(full_qry_path);

  return 0;
}