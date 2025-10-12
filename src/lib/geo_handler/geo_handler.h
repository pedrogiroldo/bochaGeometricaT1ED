/*
    This module executes the commands of the .geo file
*/
#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H
#include "../file_reader/file_reader.h"

typedef void *Ground;

/*
    @param FileData fileData - The file data with .geo file lines
    @param char* output_path - The path to the output file
    @return Ground - The ground with the shapes
*/
Ground execute_geo_commands(FileData fileData, const char *output_path);

/*
    Destroys the ground and frees the memory,
    this function should be called after the execution
    of evereything that uses the ground

    @param Ground ground - The ground to destroy
    @return void
*/
void destroy_geo_waste(Ground ground);

#endif // GEO_HANDLER_H