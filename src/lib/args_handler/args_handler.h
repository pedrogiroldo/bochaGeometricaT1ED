/*
    This module contains the functions that will
    be used to handle the arguments
    that will be passed to the program

*/
#ifndef ARGS_HANDLER_H
#define ARGS_HANDLER_H

// Declaration of the function that will be in the args_handler module
char *get_option_value(int argc, char *argv[], char *opt_name);

// Function to extract the command suffix (last string that doesn't start with
// '-')
char *get_command_suffix(int argc, char *argv[]);

#endif // ARGS_HANDLER_H
