/**
 * @file shapes.h
 * @brief Shape type enumeration
 *
 * This module defines the enumeration of all geometric shape types
 * supported in the system.
 */

#ifndef SHAPES_H
#define SHAPES_H

/**
 * @brief Enumeration of geometric shape types
 */
enum ShapeType {
    CIRCLE,      /**< Circle shape */
    RECTANGLE,   /**< Rectangle shape */
    LINE,        /**< Line shape */
    TEXT,        /**< Text shape */
    TEXT_STYLE   /**< Text style shape */
};

/**
 * @brief Type alias for ShapeType enumeration
 */
typedef enum ShapeType ShapeType;

#endif // SHAPES_H

