/**
 * Circle ADT - Represents a circle geometric element
 *
 * This module provides an abstract data type for circles with position,
 * radius, and color attributes.
 */
#ifndef CIRCLE_H
#define CIRCLE_H

/**
 * Creates a new circle instance
 * @param id Circle identifier
 * @param x X coordinate of center
 * @param y Y coordinate of center
 * @param radius Circle radius
 * @param border_color Border color string
 * @param fill_color Fill color string
 * @return Pointer to new circle or NULL on error
 */
void *circle_create(int id, double x, double y, double radius,
                    const char *border_color, const char *fill_color);

/**
 * Destroys a circle instance and frees all memory
 * @param circle Circle instance to destroy
 */
void circle_destroy(void *circle);

/**
 * Gets the circle identifier
 * @param circle Circle instance
 * @return Circle identifier
 */
int circle_get_id(void *circle);

/**
 * Gets the X coordinate of circle center
 * @param circle Circle instance
 * @return X coordinate
 */
double circle_get_x(void *circle);

/**
 * Gets the Y coordinate of circle center
 * @param circle Circle instance
 * @return Y coordinate
 */
double circle_get_y(void *circle);

/**
 * Gets the circle radius
 * @param circle Circle instance
 * @return Circle radius
 */
double circle_get_radius(void *circle);

/**
 * Gets the border color string
 * @param circle Circle instance
 * @return Border color string (do not free)
 */
const char *circle_get_border_color(void *circle);

/**
 * Gets the fill color string
 * @param circle Circle instance
 * @return Fill color string (do not free)
 */
const char *circle_get_fill_color(void *circle);

#endif // CIRCLE_H
