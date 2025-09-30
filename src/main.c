#include "lib/args_handler/args_handler.h"
#include "lib/commons/queue/queue.h"
#include "lib/file_reader/file_reader.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

  // Get arguments
  const char *output_path = get_option_value(argc, argv, "o");
  const char *geo_input_path = get_option_value(argc, argv, "f");
  const char *qry_input_path = get_option_value(argc, argv, "q");

  // Verify required arguments
  if (geo_input_path == NULL || output_path == NULL) {
    printf("Error: -f and -o are required\n");
    exit(1);
  }

  FileData file = file_data_create(geo_input_path);
  if (file == NULL) {
    printf("Error: Failed to create FileData\n");
    exit(1);
  }

  const Queue lines = get_file_lines_queue(file);
  printf("File: %s\n", get_file_name(file));
  printf("Lines: %d\n", queue_size(lines));
  printf("Peek: %s\n", (char *)queue_dequeue(lines));

  file_data_destroy(file);

  return 0;
}