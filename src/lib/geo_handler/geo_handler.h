/*
    This module executes the commands of the .geo file
*/
#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H
#include "../file_reader/file_reader.h"

typedef void* Ground;

/*
    @param FileData fileData - The file data with .geo file lines
    @return Ground - The ground with the shapes
*/
Ground execute_geo_commands(FileData fileData);

#endif // GEO_HANDLER_H