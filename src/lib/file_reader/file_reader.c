#include "file_reader.h"
#include "../commons/queue/queue.h"
#include "../commons/utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Private functions
static Queue read_file_to_queue(const char *filepath);
static char *read_line(FILE *file, char *buffer, size_t size);

struct FileData {
  const char *filepath;
  const char *filename;
  Queue linesQueue;
};

// Creates a new FileData instance and reads the file
FileData file_data_create(const char *filepath) {
  struct FileData *file = malloc(sizeof(struct FileData));
  if (file == NULL) {
    printf("Error: Failed to allocate memory for FileData\n");
    return NULL;
  }

  file->filepath = filepath;
  file->filename =
      strrchr(filepath, '/') ? strrchr(filepath, '/') + 1 : filepath;

  Queue linesQueue = read_file_to_queue(filepath);
  if (linesQueue == NULL) {
    printf("Error: Failed to read the file lines\n");
    free(file);
    return NULL;
  }
  file->linesQueue = linesQueue;
  return (FileData)file;
}

// Destroys a FileData instance and frees memory
void file_data_destroy(FileData fileData) {
  if (fileData != NULL) {
    struct FileData *file = (struct FileData *)fileData;
    if (file->linesQueue != NULL) {
      queue_destroy(file->linesQueue);
    }
    free(file);
  }
}

// Reads the file lines and returns a Queue. This function is private.
Queue read_file_to_queue(const char *filepath) {
  Queue lines = queue_create();
  FILE *file = fopen(filepath, "r");
  if (file == NULL) {
    return NULL;
  }

  char buffer[1024];
  while (read_line(file, buffer, sizeof(buffer)) != NULL) {
    queue_enqueue(lines, duplicate_string(buffer));
  }

  fclose(file);
  return lines;
}

// Gets the file path
const char *get_file_path(const FileData fileData) {
  struct FileData *file = (struct FileData *)fileData;
  return file->filepath;
}

// Gets the file name
const char *get_file_name(const FileData fileData) {
  struct FileData *file = (struct FileData *)fileData;
  return file->filename;
}

// Gets the file lines queue
const Queue get_file_lines_queue(const FileData fileData) {
  struct FileData *file = (struct FileData *)fileData;
  return file->linesQueue;
}

// Reads a line from file using fgets
static char *read_line(FILE *file, char *buffer, size_t size) {
  if (fgets(buffer, size, file) != NULL) {
    // Remove newline if present
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
    }
    return buffer;
  }
  return NULL;
}
