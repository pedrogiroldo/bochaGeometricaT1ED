/*
    This module executes the commands of the .geo file
*/
#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H
#include "../commons/queue/queue.h"

/*
    @param Queue lines - The queue with .geo file lines
    @return void - No return value
*/
void execute_geo_commands(Queue lines);

#endif // GEO_HANDLER_H