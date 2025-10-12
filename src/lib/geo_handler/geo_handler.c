#include "geo_handler.h"
#include "../commons/queue/queue.h"
#include "../commons/stack/stack.h"
#include "../file_reader/file_reader.h"
#include "../shapes/circle/circle.h"
#include "../shapes/line/line.h"
#include "../shapes/rectangle/rectangle.h"
#include "../shapes/text/text.h"
#include "../shapes/text_style/text_style.h"
#include <stdio.h>
#include <string.h>

typedef struct {
  Queue shapesQueue;
  Stack shapesStackToFree;
} Ground_t;

// private functions defined as static and implemented on the end of the file
static void execute_circle_command(Ground_t *ground);
static void execute_rectangle_command(Ground_t *ground);
static void execute_line_command(Ground_t *ground);
static void execute_text_command(Ground_t *ground);
static void execute_text_style_command(Ground_t *ground);

Ground execute_geo_commands(FileData fileData) {
  Ground_t *ground = malloc(sizeof(Ground_t));
  if (ground == NULL) {
    printf("Error: Failed to allocate memory for Ground\n");
    exit(1);
  }

  ground->shapesQueue = queue_create();
  ground->shapesStackToFree = stack_create();

  while (!queue_is_empty(get_file_lines_queue(fileData))) {
    char *line = (char *)queue_dequeue(get_file_lines_queue(fileData));
    char *command = strtok(line, " ");

    // Circle command: c i x y r corb corp
    if (strcmp(command, "c") == 0) {
      execute_circle_command(ground);
    }

    // Rectangle command: r i x y w h corb corp
    else if (strcmp(command, "r") == 0) {
      execute_rectangle_command(ground);

    }
    // Line command: l i x1 y1 x2 y2 cor
    else if (strcmp(command, "l") == 0) {
      execute_line_command(ground);

    }

    // Text command: t i x y corb corp a txto
    else if (strcmp(command, "t") == 0) {
      execute_text_command(ground);
    }

    // Text style command: ts fFamily fWeight fSize
    else if (strcmp(command, "ts") == 0) {
      execute_text_style_command(ground);
    } else {
      printf("Unknown command: %s\n", command);
    }
  }
  return ground;
}

/**
**************************
* Private functions
**************************
*/
static void execute_circle_command(Ground_t *ground) {
  char *identifier = strtok(NULL, " ");
  char *posX = strtok(NULL, " ");
  char *posY = strtok(NULL, " ");
  char *radius = strtok(NULL, " ");
  char *borderColor = strtok(NULL, " ");
  char *fillColor = strtok(NULL, " ");

  Circle circle = circle_create(atoi(identifier), atof(posX), atof(posY),
                                atof(radius), borderColor, fillColor);
}

static void execute_rectangle_command(Ground_t *ground) {
  char *identifier = strtok(NULL, " ");
  char *posX = strtok(NULL, " ");
  char *posY = strtok(NULL, " ");
  char *width = strtok(NULL, " ");
  char *height = strtok(NULL, " ");
  char *borderColor = strtok(NULL, " ");
  char *fillColor = strtok(NULL, " ");

  Rectangle rectangle =
      rectangle_create(atoi(identifier), atof(posX), atof(posY), atof(width),
                       atof(height), borderColor, fillColor);
  queue_enqueue(ground->shapesQueue, rectangle);
  stack_push(ground->shapesStackToFree, rectangle);
}

static void execute_line_command(Ground_t *ground) {
  char *identifier = strtok(NULL, " ");
  char *x1 = strtok(NULL, " ");
  char *y1 = strtok(NULL, " ");
  char *x2 = strtok(NULL, " ");
  char *y2 = strtok(NULL, " ");
  char *color = strtok(NULL, " ");

  Line line = line_create(atoi(identifier), atof(x1), atof(y1), atof(x2),
                          atof(y2), color);
  queue_enqueue(ground->shapesQueue, line);
  stack_push(ground->shapesStackToFree, line);
}

static void execute_text_command(Ground_t *ground) {
  char *identifier = strtok(NULL, " ");
  char *posX = strtok(NULL, " ");
  char *posY = strtok(NULL, " ");
  char *borderColor = strtok(NULL, " ");
  char *fillColor = strtok(NULL, " ");
  char *anchor = strtok(NULL, " ");
  char *text = strtok(NULL, "");

  Text text_obj = text_create(atoi(identifier), atof(posX), atof(posY),
                              borderColor, fillColor, *anchor, text);
  queue_enqueue(ground->shapesQueue, text_obj);
  stack_push(ground->shapesStackToFree, text_obj);
}

static void execute_text_style_command(Ground_t *ground) {
  char *fontFamily = strtok(NULL, " ");
  char *fontWeight = strtok(NULL, " ");
  char *fontSize = strtok(NULL, " ");

  TextStyle text_style_obj =
      text_style_create(fontFamily, *fontWeight, atoi(fontSize));
  queue_enqueue(ground->shapesQueue, text_style_obj);
  stack_push(ground->shapesStackToFree, text_style_obj);
}