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


#endif // FILE_READER_H
