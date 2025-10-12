#include "circle.h"
#include <stdlib.h>
#include <string.h>

/**
 * Internal Circle structure
 */
struct Circle {
  int id;
  double x;
  double y;
  double radius;
  char *border_color;
  char *fill_color;
};

/**
 * Helper function to duplicate a string (C99 compliant alternative to strdup)
 */
static char *duplicate_string(const char *s) {
  if (!s)
    return NULL;

  size_t len = strlen(s) + 1;
  char *dup = malloc(len);
  if (dup) {
    strcpy(dup, s);
  }
  return dup;
}

void *circle_create(int id, double x, double y, double radius,
                    const char *border_color, const char *fill_color) {
  if (!border_color || !fill_color) {
    return NULL;
  }

  struct Circle *circle = malloc(sizeof(struct Circle));
  if (!circle) {
    return NULL;
  }

  circle->id = id;
  circle->x = x;
  circle->y = y;
  circle->radius = radius;

  circle->border_color = duplicate_string(border_color);
  if (!circle->border_color) {
    free(circle);
    return NULL;
  }

  circle->fill_color = duplicate_string(fill_color);
  if (!circle->fill_color) {
    free(circle->border_color);
    free(circle);
    return NULL;
  }

  return circle;
}

void circle_destroy(void *circle) {
  if (!circle)
    return;

  struct Circle *c = (struct Circle *)circle;
  free(c->border_color);
  free(c->fill_color);
  free(c);
}

int circle_get_id(void *circle) {
  if (!circle)
    return -1;
  return ((struct Circle *)circle)->id;
}

double circle_get_x(void *circle) {
  if (!circle)
    return 0.0;
  return ((struct Circle *)circle)->x;
}

double circle_get_y(void *circle) {
  if (!circle)
    return 0.0;
  return ((struct Circle *)circle)->y;
}

double circle_get_radius(void *circle) {
  if (!circle)
    return 0.0;
  return ((struct Circle *)circle)->radius;
}

const char *circle_get_border_color(void *circle) {
  if (!circle)
    return NULL;
  return ((struct Circle *)circle)->border_color;
}

const char *circle_get_fill_color(void *circle) {
  if (!circle)
    return NULL;
  return ((struct Circle *)circle)->fill_color;
}
