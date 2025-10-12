/**
 * Rectangle ADT - Represents a rectangle geometric element
 *
 * This module provides an abstract data type for rectangles with position,
 * dimensions, and color attributes.
 */
#ifndef RECTANGLE_H
#define RECTANGLE_H

typedef void* Rectangle;

/**
 * Creates a new rectangle instance
 * @param id Rectangle identifier
 * @param x X coordinate of anchor point
 * @param y Y coordinate of anchor point
 * @param width Rectangle width
 * @param height Rectangle height
 * @param border_color Border color string
 * @param fill_color Fill color string
 * @return Pointer to new rectangle or NULL on error
 */
Rectangle rectangle_create(int id, double x, double y, double width, double height,
                       const char *border_color, const char *fill_color);

/**
 * Destroys a rectangle instance and frees all memory
 * @param rectangle Rectangle instance to destroy
 */
void rectangle_destroy(Rectangle rectangle);

/**
 * Gets the rectangle identifier
 * @param rectangle Rectangle instance
 * @return Rectangle identifier
 */
int rectangle_get_id(Rectangle rectangle);

/**
 * Gets the X coordinate of rectangle anchor
 * @param rectangle Rectangle instance
 * @return X coordinate
 */
double rectangle_get_x(Rectangle rectangle);

/**
 * Gets the Y coordinate of rectangle anchor
 * @param rectangle Rectangle instance
 * @return Y coordinate
 */
double rectangle_get_y(Rectangle rectangle);

/**
 * Gets the rectangle width
 * @param rectangle Rectangle instance
 * @return Rectangle width
 */
double rectangle_get_width(Rectangle rectangle);

/**
 * Gets the rectangle height
 * @param rectangle Rectangle instance
 * @return Rectangle height
 */
double rectangle_get_height(Rectangle rectangle);

/**
 * Gets the border color string
 * @param rectangle Rectangle instance
 * @return Border color string (do not free)
 */
const char *rectangle_get_border_color(Rectangle rectangle);

/**
 * Gets the fill color string
 * @param rectangle Rectangle instance
 * @return Fill color string (do not free)
 */
const char *rectangle_get_fill_color(Rectangle rectangle);

#endif // RECTANGLE_H
