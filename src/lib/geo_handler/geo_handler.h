/*
    This module executes the commands of the .geo file
*/
#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H
#include "../file_reader/file_reader.h"

/*
    @param FileData file - The file data
    @return void - No return value
*/
void execute_geo_commands(FileData file);

#endif // GEO_HANDLER_H