#include "file_reader.h"
#include "../commons/queue/queue.h"
#include <stdio.h>
#include <string.h>

// Private functions
static Queue *read_file_lines(const char *filepath);

typedef struct FileData {
  const char *filepath;
  const char *filename;
  Queue *linesQueue;
} FileData;

// Reads the file and returns a FileData struct
FileData read_file(const char *filepath) {
  FileData file;
  file.filepath = filepath;
  file.filename =
      strrchr(filepath, '/') ? strrchr(filepath, '/') + 1 : filepath;
  Queue *linesQueue = read_file_lines(filepath);
  if (linesQueue == NULL) {
    printf("Error: Failed to read the file lines\n");
    exit(1);
  }
  file.linesQueue = linesQueue;
  return file;
}

// Reads the file lines and returns a Queue. This function is private.
Queue *read_file_lines(const char *filepath) {
  Queue *lines = queue_create();
  FILE *file = fopen(filepath, "r");
  if (file == NULL) {
    return NULL;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  while ((read = getline(&line, &len, file)) != -1) {
    queue_enqueue(lines, strdup(line));
  }

  free(line); // Free the buffer allocated by getline
  fclose(file);
  return lines;
}

// Gets the file path
const char *get_file_path(const FileData *fileData) {
  return fileData->filepath;
}

// Gets the file name
const char *get_file_name(const FileData *fileData) {
  return fileData->filename;
}

// Gets the file lines queue
const Queue *get_file_lines_queue(const FileData *fileData) {
  return fileData->linesQueue;
}