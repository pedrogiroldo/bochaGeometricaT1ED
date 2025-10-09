/*
    This module contain the functions that will be
    used to read the .geo and .qry files.
*/
#ifndef FILE_READER_H
#define FILE_READER_H

#include "../commons/queue/queue.h"

typedef void *FileData;

// Creates a new FileData instance and reads the file
FileData file_data_create(const char *filepath);

// Destroys a FileData instance and frees memory
void file_data_destroy(FileData fileData);

// Gets the file path
const char *get_file_path(const FileData fileData);

// Gets the file name
const char *get_file_name(const FileData fileData);

// Gets the file lines queue
Queue get_file_lines_queue(const FileData fileData);

#endif // FILE_READER_H
