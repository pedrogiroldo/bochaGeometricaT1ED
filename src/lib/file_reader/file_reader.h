/*
    This module contain the functions that will be
    used to read the .geo and .qry files.
*/
#ifndef FILE_READER_H
#define FILE_READER_H

#include "../commons/queue/queue.h"

typedef struct FileData FileData;
// Reads the file and returns a FileData struct
FileData read_file(const char *filepath);

// Gets the file path
const char *get_file_path(const FileData *fileData);

// Gets the file name
const char *get_file_name(const FileData *fileData);

// Gets the file lines
const Queue *get_file_lines_queue(const FileData *fileData);

#endif // FILE_READER_H
