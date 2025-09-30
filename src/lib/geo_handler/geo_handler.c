#include "geo_handler.h"
#include "../commons/queue/queue.h"
#include "../file_reader/file_reader.h"
#include <stdio.h>

void execute_geo_commands(FileData file) {
  const Queue lines = get_file_lines_queue(file);
  while (!queue_is_empty(lines)) {
    const char *line = (char *)queue_dequeue(lines);
    printf("%s\n", line);
  }
}
