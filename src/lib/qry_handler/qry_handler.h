/*
    This module executes the commands of the .qry file
*/
#ifndef QRY_HANDLER_H
#define QRY_HANDLER_H
#include "../file_reader/file_reader.h"
#include "../geo_handler/geo_handler.h"

typedef void *Qry;

void execute_qry_commands(FileData fileData, Ground ground,
                          const char *output_path, const char *command_suffix);

void destroy_qry_waste(Qry qry);

#endif // QRY_HANDLER_H