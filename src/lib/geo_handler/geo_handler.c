#include "geo_handler.h"
#include "../commons/queue/queue.h"
#include "../commons/stack/stack.h"
#include "../file_reader/file_reader.h"
#include "../shapes/circle/circle.h"
#include "../shapes/line/line.h"
#include "../shapes/rectangle/rectangle.h"
#include "../shapes/shape.h"
#include "../shapes/text/text.h"
#include "../shapes/text_style/text_style.h"
#include <stdio.h>
#include <string.h>

typedef struct {
  Queue shapesQueue;
  Stack shapesStackToFree;
  Queue svgQueue;
} Ground_t;

typedef struct {
  ShapeType type;
  void *data;
} Shape_t;

// private functions defined as static and implemented on the end of the file
static void execute_circle_command(Ground_t *ground);
static void execute_rectangle_command(Ground_t *ground);
static void execute_line_command(Ground_t *ground);
static void execute_text_command(Ground_t *ground);
static void execute_text_style_command(Ground_t *ground);
static void create_svg_queue(Ground_t *ground, const char *output_path,
                             FileData fileData);

Ground execute_geo_commands(FileData fileData, const char *output_path) {
  Ground_t *ground = malloc(sizeof(Ground_t));
  if (ground == NULL) {
    printf("Error: Failed to allocate memory for Ground\n");
    exit(1);
  }

  ground->shapesQueue = queue_create();
  ground->shapesStackToFree = stack_create();
  ground->svgQueue = queue_create();
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
  create_svg_queue(ground, output_path, fileData);
  return ground;
}

void destroy_geo_waste(Ground ground) {
  Ground_t *ground_t = (Ground_t *)ground;
  queue_destroy(ground_t->shapesQueue);
  queue_destroy(ground_t->svgQueue);
  while (!stack_is_empty(ground_t->shapesStackToFree)) {
    Shape_t *shape = stack_pop(ground_t->shapesStackToFree);
    switch (shape->type) {
    case CIRCLE:
      circle_destroy(shape->data);
      break;
    case RECTANGLE:
      rectangle_destroy(shape->data);
      break;
    case LINE:
      line_destroy(shape->data);
      break;
    case TEXT:
      text_destroy(shape->data);
      break;
    case TEXT_STYLE:
      text_style_destroy(shape->data);
      break;
    default:
      break;
    }
    free(shape);
  }
  stack_destroy(ground_t->shapesStackToFree);
  free(ground);
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

  Shape_t *shape = malloc(sizeof(Shape_t));
  if (shape == NULL) {
    printf("Error: Failed to allocate memory for Shape\n");
    exit(1);
  }
  shape->type = CIRCLE;
  shape->data = circle;
  queue_enqueue(ground->shapesQueue, shape);
  stack_push(ground->shapesStackToFree, shape);
  queue_enqueue(ground->svgQueue, shape);
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

  Shape_t *shape = malloc(sizeof(Shape_t));
  if (shape == NULL) {
    printf("Error: Failed to allocate memory for Shape\n");
    exit(1);
  }
  shape->type = RECTANGLE;
  shape->data = rectangle;
  queue_enqueue(ground->shapesQueue, shape);
  stack_push(ground->shapesStackToFree, shape);
  queue_enqueue(ground->svgQueue, shape);
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

  Shape_t *shape = malloc(sizeof(Shape_t));
  if (shape == NULL) {
    printf("Error: Failed to allocate memory for Shape\n");
    exit(1);
  }
  shape->type = LINE;
  shape->data = line;
  queue_enqueue(ground->shapesQueue, shape);
  stack_push(ground->shapesStackToFree, shape);
  queue_enqueue(ground->svgQueue, shape);
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

  Shape_t *shape = malloc(sizeof(Shape_t));
  if (shape == NULL) {
    printf("Error: Failed to allocate memory for Shape\n");
    exit(1);
  }
  shape->type = TEXT;
  shape->data = text_obj;
  queue_enqueue(ground->shapesQueue, shape);
  stack_push(ground->shapesStackToFree, shape);
  queue_enqueue(ground->svgQueue, shape);
}

static void execute_text_style_command(Ground_t *ground) {
  char *fontFamily = strtok(NULL, " ");
  char *fontWeight = strtok(NULL, " ");
  char *fontSize = strtok(NULL, " ");

  TextStyle text_style_obj =
      text_style_create(fontFamily, *fontWeight, atoi(fontSize));

  Shape_t *shape = malloc(sizeof(Shape_t));
  if (shape == NULL) {
    printf("Error: Failed to allocate memory for Shape\n");
    exit(1);
  }
  shape->type = TEXT_STYLE;
  shape->data = text_style_obj;
  queue_enqueue(ground->shapesQueue, shape);
  stack_push(ground->shapesStackToFree, shape);
  queue_enqueue(ground->svgQueue, shape);
}

static void create_svg_queue(Ground_t *ground, const char *output_path,
                             FileData fileData) {
  const char *original_file_name = get_file_name(fileData);
  size_t name_len = strlen(original_file_name);
  char *file_name = malloc(name_len + 1);
  if (file_name == NULL) {
    printf("Error: Memory allocation failed for file name\n");
    return;
  }
  strcpy(file_name, original_file_name);
  strtok(file_name, ".");

  // Calculate required buffer size: output_path + "/" + file_name + ".svg" +
  // null terminator
  size_t path_len = strlen(output_path);
  size_t processed_name_len = strlen(file_name);
  size_t total_len = path_len + 1 + processed_name_len + 4 +
                     1; // +1 for "/", +4 for ".svg", +1 for null terminator

  // Use dynamic allocation for safety
  char *output_path_with_file = malloc(total_len);
  if (output_path_with_file == NULL) {
    printf("Error: Memory allocation failed\n");
    return;
  }

  // Use snprintf for safe string construction
  int result = snprintf(output_path_with_file, total_len, "%s/%s.svg",
                        output_path, file_name);
  if (result < 0 || (size_t)result >= total_len) {
    printf("Error: Path construction failed\n");
    free(output_path_with_file);
    return;
  }

  FILE *file = fopen(output_path_with_file, "w");
  if (file == NULL) {
    printf("Error: Failed to open file: %s\n", output_path_with_file);
    free(output_path_with_file);
    return;
  }
  fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  fprintf(
      file,
      "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 1000 1000\">\n");
  while (!queue_is_empty(ground->svgQueue)) {
    Shape_t *shape = queue_dequeue(ground->svgQueue);
    if (shape != NULL) {
      if (shape->type == CIRCLE) {
        Circle circle = (Circle)shape->data;
        fprintf(
            file,
            "<circle cx='%.2f' cy='%.2f' r='%.2f' fill='%s' stroke='%s'/>\n",
            circle_get_x(circle), circle_get_y(circle),
            circle_get_radius(circle), circle_get_fill_color(circle),
            circle_get_border_color(circle));
      } else if (shape->type == RECTANGLE) {
        Rectangle rectangle = (Rectangle)shape->data;
        fprintf(file,
                "<rect x='%.2f' y='%.2f' width='%.2f' height='%.2f' fill='%s' "
                "stroke='%s'/>\n",
                rectangle_get_x(rectangle), rectangle_get_y(rectangle),
                rectangle_get_width(rectangle), rectangle_get_height(rectangle),
                rectangle_get_fill_color(rectangle),
                rectangle_get_border_color(rectangle));
      } else if (shape->type == LINE) {
        Line line = (Line)shape->data;
        fprintf(file,
                "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s'/>\n",
                line_get_x1(line), line_get_y1(line), line_get_x2(line),
                line_get_y2(line), line_get_color(line));
      } else if (shape->type == TEXT) {
        Text text = (Text)shape->data;
        char anchor = text_get_anchor(text);
        const char *text_anchor = "start"; // default

        // Map anchor character to SVG text-anchor value
        if (anchor == 'm' || anchor == 'M') {
          text_anchor = "middle";
        } else if (anchor == 'e' || anchor == 'E') {
          text_anchor = "end";
        } else if (anchor == 's' || anchor == 'S') {
          text_anchor = "start";
        }

        fprintf(file,
                "<text x='%.2f' y='%.2f' fill='%s' stroke='%s' "
                "text-anchor='%s'>%s</text>\n",
                text_get_x(text), text_get_y(text), text_get_fill_color(text),
                text_get_border_color(text), text_anchor, text_get_text(text));
      }
    }
  }
  fprintf(file, "</svg>\n");
  fclose(file);
  free(output_path_with_file);
  free(file_name);
}