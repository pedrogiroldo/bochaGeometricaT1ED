/*
    This module executes the commands of the .geo file
*/
#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H
#include "../commons/stack/stack.h"
#include "../file_reader/file_reader.h"

typedef void *Ground;

/*
    @param FileData fileData - The file data with .geo file lines
    @param char* output_path - The path to the output file
    @param char* command_suffix - The command suffix to add to the output file
   name
    @return Ground - The ground with the shapes
*/
Ground execute_geo_commands(FileData fileData, const char *output_path,
                            const char *command_suffix);

/*
    Gets the ground queue
    @param Ground ground - The ground to get the queue from
    @return Queue - The ground queue
*/
Queue get_ground_queue(Ground ground);

/*
    Gets the ground shapes stack to free
    @param Ground ground - The ground to get the stack from
    @return Stack - The shapes stack to free
*/
Stack get_ground_shapes_stack_to_free(Ground ground);

/*
    Destroys the ground and frees the memory,
    this function should be called after the execution
    of evereything that uses the ground

    @param Ground ground - The ground to destroy
    @return void
*/
void destroy_geo_waste(Ground ground);

#endif // GEO_HANDLER_H