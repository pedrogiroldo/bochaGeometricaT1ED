/*
    This module executes the commands of the .geo file
*/
#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H
#include "../file_reader/file_reader.h"

typedef void* Ground;

/*
    @param FileData fileData - The file data with .geo file lines
    @param char* output_path - The path to the output file
    @return Ground - The ground with the shapes
*/
Ground execute_geo_commands(FileData fileData, const char* output_path);

#endif // GEO_HANDLER_H