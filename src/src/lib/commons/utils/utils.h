/*
 * General utility functions
 */
#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>

/**
 * Duplicates a string using malloc
 * @param s Source string to duplicate
 * @return New string or NULL on error
 */
char *duplicate_string(const char *s);

/**
 * Produces the inverted color for a given color string.
 * Supports 6-digit hex colors (e.g., "#aabbcc") and a small set of common
 * named colors used in inputs (e.g., "red", "blue", "green", "yellow",
 * "black", "pink", "cyan", "orange", "teal", "purple").
 * For recognized inputs, returns a newly allocated string in the form
 * "#RRGGBB" representing the inverted color. For unrecognized inputs,
 * returns a duplicate of the original color.
 * Caller must free the returned string using free().
 */
char *invert_color(const char *color);

#endif // UTILS_H