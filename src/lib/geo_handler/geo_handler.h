/**
 * @file geo_handler.h
 * @brief Geographic file command handler
 *
 * This module executes the commands from .geo files and manages
 * the ground state with all geometric shapes.
 */

#ifndef GEO_HANDLER_H
#define GEO_HANDLER_H
#include "../commons/stack/stack.h"
#include "../file_reader/file_reader.h"

/**
 * @brief Opaque pointer type for ground instances
 */
typedef void *Ground;

/**
 * @brief Executes commands from a .geo file and creates the ground
 * @param fileData File data containing .geo file lines
 * @param output_path Path to the output file
 * @param command_suffix Command suffix to add to the output file name
 * @return Ground instance with all shapes or NULL on error
 */
Ground execute_geo_commands(FileData fileData, const char *output_path,
                            const char *command_suffix);

/**
 * @brief Gets the ground queue containing all shapes
 * @param ground Ground instance
 * @return Queue containing all shapes
 */
Queue get_ground_queue(Ground ground);

/**
 * @brief Gets the ground shapes stack used for memory management
 * @param ground Ground instance
 * @return Stack containing shapes to free
 */
Stack get_ground_shapes_stack_to_free(Ground ground);

/**
 * @brief Destroys the ground and frees all associated memory
 *
 * This function should be called after all operations that use the ground
 * are completed to prevent memory leaks.
 *
 * @param ground Ground instance to destroy
 */
void destroy_geo_waste(Ground ground);

#endif // GEO_HANDLER_H