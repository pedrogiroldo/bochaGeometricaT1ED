#include "rectangle.h"
#include "../../commons/utils/utils.h"
#include <stdlib.h>
#include <string.h>

/**
 * Internal Rectangle structure
 */
struct Rectangle {
  int id;
  double x;
  double y;
  double width;
  double height;
  char *border_color;
  char *fill_color;
};

void *rectangle_create(int id, double x, double y, double width, double height,
                       const char *border_color, const char *fill_color) {
  if (!border_color || !fill_color) {
    return NULL;
  }

  struct Rectangle *rectangle = malloc(sizeof(struct Rectangle));
  if (!rectangle) {
    return NULL;
  }

  rectangle->id = id;
  rectangle->x = x;
  rectangle->y = y;
  rectangle->width = width;
  rectangle->height = height;

  rectangle->border_color = duplicate_string(border_color);
  if (!rectangle->border_color) {
    free(rectangle);
    return NULL;
  }

  rectangle->fill_color = duplicate_string(fill_color);
  if (!rectangle->fill_color) {
    free(rectangle->border_color);
    free(rectangle);
    return NULL;
  }

  return rectangle;
}

void rectangle_destroy(void *rectangle) {
  if (!rectangle)
    return;

  struct Rectangle *r = (struct Rectangle *)rectangle;
  free(r->border_color);
  free(r->fill_color);
  free(r);
}

int rectangle_get_id(void *rectangle) {
  if (!rectangle)
    return -1;
  return ((struct Rectangle *)rectangle)->id;
}

double rectangle_get_x(void *rectangle) {
  if (!rectangle)
    return 0.0;
  return ((struct Rectangle *)rectangle)->x;
}

double rectangle_get_y(void *rectangle) {
  if (!rectangle)
    return 0.0;
  return ((struct Rectangle *)rectangle)->y;
}

double rectangle_get_width(void *rectangle) {
  if (!rectangle)
    return 0.0;
  return ((struct Rectangle *)rectangle)->width;
}

double rectangle_get_height(void *rectangle) {
  if (!rectangle)
    return 0.0;
  return ((struct Rectangle *)rectangle)->height;
}

const char *rectangle_get_border_color(void *rectangle) {
  if (!rectangle)
    return NULL;
  return ((struct Rectangle *)rectangle)->border_color;
}

const char *rectangle_get_fill_color(void *rectangle) {
  if (!rectangle)
    return NULL;
  return ((struct Rectangle *)rectangle)->fill_color;
}
