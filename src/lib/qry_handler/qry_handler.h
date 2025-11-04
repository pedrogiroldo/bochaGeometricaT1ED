/**
 * @file qry_handler.h
 * @brief Query file command handler
 *
 * This module executes the commands from .qry files and processes
 * queries on the geometric shapes loaded from .geo files.
 */

#ifndef QRY_HANDLER_H
#define QRY_HANDLER_H
#include "../file_reader/file_reader.h"
#include "../geo_handler/geo_handler.h"

/**
 * @brief Opaque pointer type for query instances
 */
typedef void *Qry;

/**
 * @brief Executes commands from a .qry file and processes queries
 * @param qryFileData File data containing .qry file lines
 * @param geoFileData File data containing .geo file lines
 * @param ground Ground instance with all geometric shapes
 * @param output_path Path to the output file
 * @return Qry instance or NULL on error
 */
Qry execute_qry_commands(FileData qryFileData, FileData geoFileData,
                         Ground ground, const char *output_path);

/**
 * @brief Destroys the query instance and frees all associated memory
 *
 * This function should be called after all query operations are completed
 * to prevent memory leaks.
 *
 * @param qry Query instance to destroy
 */
void destroy_qry_waste(Qry qry);

#endif // QRY_HANDLER_H