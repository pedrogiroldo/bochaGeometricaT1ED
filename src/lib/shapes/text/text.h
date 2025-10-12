/**
 * Text ADT - Represents a text geometric element
 *
 * This module provides an abstract data type for text elements with position,
 * colors, anchor, and text content.
 */
#ifndef TEXT_H
#define TEXT_H

/**
 * Creates a new text instance
 * @param id Text identifier
 * @param x X coordinate of text position
 * @param y Y coordinate of text position
 * @param border_color Border color string
 * @param fill_color Fill color string
 * @param anchor Text anchor character
 * @param text Text content string
 * @return Pointer to new text or NULL on error
 */
void *text_create(int id, double x, double y, const char *border_color,
                  const char *fill_color, char anchor, const char *text);

/**
 * Destroys a text instance and frees all memory
 * @param text Text instance to destroy
 */
void text_destroy(void *text);

/**
 * Gets the text identifier
 * @param text Text instance
 * @return Text identifier
 */
int text_get_id(void *text);

/**
 * Gets the X coordinate of text position
 * @param text Text instance
 * @return X coordinate
 */
double text_get_x(void *text);

/**
 * Gets the Y coordinate of text position
 * @param text Text instance
 * @return Y coordinate
 */
double text_get_y(void *text);

/**
 * Gets the border color string
 * @param text Text instance
 * @return Border color string (do not free)
 */
const char *text_get_border_color(void *text);

/**
 * Gets the fill color string
 * @param text Text instance
 * @return Fill color string (do not free)
 */
const char *text_get_fill_color(void *text);

/**
 * Gets the text anchor character
 * @param text Text instance
 * @return Anchor character
 */
char text_get_anchor(void *text);

/**
 * Gets the text content string
 * @param text Text instance
 * @return Text content string (do not free)
 */
const char *text_get_text(void *text);

#endif // TEXT_H
