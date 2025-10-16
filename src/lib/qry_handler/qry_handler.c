#include "qry_handler.h"
#include "../commons/queue/queue.h"
#include "../commons/stack/stack.h"
#include "../geo_handler/geo_handler.h"
#include "../shapes/circle/circle.h"
#include "../shapes/line/line.h"
#include "../shapes/rectangle/rectangle.h"
#include "../shapes/shapes.h"
#include "../shapes/text/text.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  ShapeType type;
  void *data;
} Shape_t;

typedef struct {
  int id;
  Stack *shapes;
} Loader_t;

typedef struct {
  int id;
  double x;
  double y;
  Shape_t *shootingPosition;
  Loader_t *rightLoader;
  Loader_t *leftLoader;
  int rightLoaderId; // stable identifier, avoids dangling pointers after
                     // realloc
  int leftLoaderId; // stable identifier, avoids dangling pointers after realloc
} Shooter_t;

typedef struct {
  Stack arena; // elements are ShapePositionOnArena_t
  Stack stackToFree;
} Qry_t;

typedef struct {
  Shape_t *shape;
  double x;
  double y;
  bool isAnnotated;
  double shooterX;
  double shooterY;
} ShapePositionOnArena_t;

// private functions
static void execute_pd_command(Shooter_t **shooters, int *shootersCount,
                               Stack stackToFree);
static void execute_lc_command(Loader_t **loaders, int *loadersCount,
                               Ground ground, Stack stackToFree);
static void execute_atch_command(Loader_t **loaders, int *loadersCount,
                                 Shooter_t **shooters, int *shootersCount,
                                 Stack stackToFree);
static void perform_shift_operation(Shooter_t **shooters, int shootersCount,
                                    int shooterId, const char *direction,
                                    int times, Loader_t *loaders,
                                    int loadersCount);
static void perform_shoot_operation(Shooter_t **shooters, int shootersCount,
                                    int shooterId, double dx, double dy,
                                    const char *annotate, Stack arena,
                                    Stack stackToFree);
static void execute_shft_command(Shooter_t **shooters, int *shootersCount,
                                 Loader_t *loaders, int *loadersCount);
static void execute_dsp_command(Shooter_t **shooters, int *shootersCount,
                                Stack arena, Stack stackToFree);
static void execute_rjd_command(Shooter_t **shooters, int *shootersCount,
                                Stack stackToFree, Stack arena,
                                Loader_t *loaders, int *loadersCount);
static void execute_calc_command(Stack arena, Ground ground);
static int find_shooter_index_by_id(Shooter_t **shooters, int shootersCount,
                                    int id);

// Helpers for calc
static double shape_area(ShapeType type, void *shapeData);
typedef struct {
  double minX;
  double minY;
  double maxX;
  double maxY;
} Aabb;
static Aabb make_aabb_for_shape_on_arena(const ShapePositionOnArena_t *s);
static bool aabb_overlap(Aabb a, Aabb b);
static bool shapes_overlap(const ShapePositionOnArena_t *a,
                           const ShapePositionOnArena_t *b);
static Shape_t *make_shape_wrapper(ShapeType type, void *data);
static Shape_t *clone_with_border_color(Shape_t *src,
                                        const char *newBorderColor);
static Shape_t *clone_with_swapped_colors(Shape_t *src);
// Clone helpers setting a new position (x,y) based on arena placement
static Shape_t *clone_with_position(Shape_t *src, double x, double y);
static Shape_t *clone_with_border_color_at_position(Shape_t *src,
                                                    const char *newBorderColor,
                                                    double x, double y);
static Shape_t *clone_with_swapped_colors_at_position(Shape_t *src, double x,
                                                      double y);
static void destroy_shape(Shape_t *s);

// SVG writer for final .qry result
static void write_qry_result_svg(FileData qryFileData, FileData geoFileData,
                                 Ground ground, Stack arena,
                                 const char *output_path);

void execute_qry_commands(FileData qryFileData, FileData geoFileData,
                          Ground ground, const char *output_path) {

  Qry_t *qry = malloc(sizeof(Qry_t));
  if (qry == NULL) {
    printf("Error: Failed to allocate memory for Qry\n");
    exit(1);
  }
  qry->arena = stack_create();
  qry->stackToFree = stack_create();

  // Add qry to stackToFree for cleanup
  stack_push(qry->stackToFree, qry);

  Shooter_t *shooters = NULL;
  int shootersCount = 0;
  Loader_t *loaders = NULL;
  int loadersCount = 0;

  while (!queue_is_empty(get_file_lines_queue(qryFileData))) {
    char *line = (char *)queue_dequeue(get_file_lines_queue(qryFileData));
    char *command = strtok(line, " \t\r\n");

    if (command == NULL || *command == '\0') {
      continue;
    }

    if (strcmp(command, "pd") == 0) {
      execute_pd_command(&shooters, &shootersCount, qry->stackToFree);
    } else if (strcmp(command, "lc") == 0) {
      execute_lc_command(&loaders, &loadersCount, ground, qry->stackToFree);
    } else if (strcmp(command, "atch") == 0) {
      execute_atch_command(&loaders, &loadersCount, &shooters, &shootersCount,
                           qry->stackToFree);
    } else if (strcmp(command, "shft") == 0) {
      execute_shft_command(&shooters, &shootersCount, loaders, &loadersCount);
    } else if (strcmp(command, "dsp") == 0) {
      execute_dsp_command(&shooters, &shootersCount, qry->arena,
                          qry->stackToFree);
    } else if (strcmp(command, "rjd") == 0) {
      execute_rjd_command(&shooters, &shootersCount, qry->stackToFree,
                          qry->arena, loaders, &loadersCount);
    } else if (strcmp(command, "calc") == 0) {
      execute_calc_command(qry->arena, ground);
    } else
      printf("Unknown command: %s\n", command);
  }

  // After processing all commands, emit final SVG with remaining ground shapes
  // and visual annotations derived from the arena
  write_qry_result_svg(qryFileData, geoFileData, ground, qry->arena,
                       output_path);
}

/*
==========================
Private functions
==========================
*/

static void execute_pd_command(Shooter_t **shooters, int *shootersCount,
                               Stack stackToFree) {
  char *identifier = strtok(NULL, " ");
  char *posX = strtok(NULL, " ");
  char *posY = strtok(NULL, " ");

  *shootersCount += 1;

  *shooters = realloc(*shooters, *shootersCount * sizeof(Shooter_t));
  if (*shooters == NULL) {
    printf("Error: Failed to allocate memory for Shooters\n");
    exit(1);
  }

  // Add shooters array to stackToFree for cleanup
  stack_push(stackToFree, *shooters);
  (*shooters)[*shootersCount - 1] = (Shooter_t){.id = atoi(identifier),
                                                .x = atof(posX),
                                                .y = atof(posY),
                                                .shootingPosition = NULL,
                                                .rightLoader = NULL,
                                                .leftLoader = NULL,
                                                .rightLoaderId = -1,
                                                .leftLoaderId = -1};
}

static void execute_lc_command(Loader_t **loaders, int *loadersCount,
                               Ground ground, Stack stackToFree) {
  char *identifier = strtok(NULL, " ");
  char *firstXShapes = strtok(NULL, " ");

  int loaderId = atoi(identifier);
  int newShapesCount = atoi(firstXShapes);

  // Check if loader already exists
  int existingLoaderIndex = -1;
  for (int i = 0; i < *loadersCount; i++) {
    if ((*loaders)[i].id == loaderId) {
      existingLoaderIndex = i;
      break;
    }
  }

  if (existingLoaderIndex == -1) {
    // Create new loader
    *loadersCount += 1;
    *loaders = realloc(*loaders, *loadersCount * sizeof(Loader_t));
    if (*loaders == NULL) {
      printf("Error: Failed to allocate memory for Loaders\n");
      exit(1);
    }

    // Add loaders array to stackToFree for cleanup
    stack_push(stackToFree, *loaders);
    (*loaders)[*loadersCount - 1] = (Loader_t){.id = loaderId, .shapes = NULL};
    existingLoaderIndex = *loadersCount - 1;
  }

  // Create stack if it doesn't exist
  if ((*loaders)[existingLoaderIndex].shapes == NULL) {
    (*loaders)[existingLoaderIndex].shapes = malloc(sizeof(Stack));
    if ((*loaders)[existingLoaderIndex].shapes == NULL) {
      printf("Error: Failed to allocate memory for Loader stack\n");
      exit(1);
    }

    // Add shapes stack to stackToFree for cleanup
    stack_push(stackToFree, (*loaders)[existingLoaderIndex].shapes);
    *(*loaders)[existingLoaderIndex].shapes = stack_create();
    if (*(*loaders)[existingLoaderIndex].shapes == NULL) {
      printf("Error: Failed to create stack for Loader\n");
      exit(1);
    }
  }

  // Add new shapes to the stack
  for (int i = 0; i < newShapesCount; i++) {
    Shape_t *shape = queue_dequeue(get_ground_queue(ground));
    if (shape != NULL) {
      if (!stack_push(*(*loaders)[existingLoaderIndex].shapes, shape)) {
        printf("Error: Failed to push shape to loader stack\n");
        exit(1);
      }
    }
  }
}

static void execute_atch_command(Loader_t **loaders, int *loadersCount,
                                 Shooter_t **shooters, int *shootersCount,
                                 Stack stackToFree) {
  char *shooterId = strtok(NULL, " ");
  char *leftLoaderId = strtok(NULL, " ");
  char *rightLoaderId = strtok(NULL, " ");

  int shooterIdInt = atoi(shooterId);
  int leftLoaderIdInt = atoi(leftLoaderId);
  int rightLoaderIdInt = atoi(rightLoaderId);

  int shooterIndex =
      find_shooter_index_by_id(shooters, *shootersCount, shooterIdInt);
  if (shooterIndex != -1) {
    // Find left and right loaders by id; create empty ones if not found
    Loader_t *leftLoaderPtr = NULL;
    Loader_t *rightLoaderPtr = NULL;
    for (int j = 0; j < *loadersCount; j++) {
      if ((*loaders)[j].id == leftLoaderIdInt) {
        leftLoaderPtr = &(*loaders)[j];
      }
      if ((*loaders)[j].id == rightLoaderIdInt) {
        rightLoaderPtr = &(*loaders)[j];
      }
    }

    // Create left loader if it does not exist
    if (leftLoaderPtr == NULL) {
      *loadersCount += 1;
      *loaders = realloc(*loaders, *loadersCount * sizeof(Loader_t));
      if (*loaders == NULL) {
        printf("Error: Failed to allocate memory for Loaders\n");
        exit(1);
      }
      // Track the (re)allocated loaders array for later cleanup
      stack_push(stackToFree, *loaders);
      (*loaders)[*loadersCount - 1] =
          (Loader_t){.id = leftLoaderIdInt, .shapes = NULL};
      // Create empty shapes stack for the new loader
      (*loaders)[*loadersCount - 1].shapes = malloc(sizeof(Stack));
      if ((*loaders)[*loadersCount - 1].shapes == NULL) {
        printf("Error: Failed to allocate memory for Loader stack\n");
        exit(1);
      }
      stack_push(stackToFree, (*loaders)[*loadersCount - 1].shapes);
      *(*loaders)[*loadersCount - 1].shapes = stack_create();
      if (*(*loaders)[*loadersCount - 1].shapes == NULL) {
        printf("Error: Failed to create stack for Loader\n");
        exit(1);
      }
      leftLoaderPtr = &(*loaders)[*loadersCount - 1];
    }

    // Create right loader if it does not exist
    if (rightLoaderPtr == NULL) {
      *loadersCount += 1;
      *loaders = realloc(*loaders, *loadersCount * sizeof(Loader_t));
      if (*loaders == NULL) {
        printf("Error: Failed to allocate memory for Loaders\n");
        exit(1);
      }
      // Track the (re)allocated loaders array for later cleanup
      stack_push(stackToFree, *loaders);
      (*loaders)[*loadersCount - 1] =
          (Loader_t){.id = rightLoaderIdInt, .shapes = NULL};
      // Create empty shapes stack for the new loader
      (*loaders)[*loadersCount - 1].shapes = malloc(sizeof(Stack));
      if ((*loaders)[*loadersCount - 1].shapes == NULL) {
        printf("Error: Failed to allocate memory for Loader stack\n");
        exit(1);
      }
      stack_push(stackToFree, (*loaders)[*loadersCount - 1].shapes);
      *(*loaders)[*loadersCount - 1].shapes = stack_create();
      if (*(*loaders)[*loadersCount - 1].shapes == NULL) {
        printf("Error: Failed to create stack for Loader\n");
        exit(1);
      }
      rightLoaderPtr = &(*loaders)[*loadersCount - 1];
    }

    (*shooters)[shooterIndex].leftLoader = leftLoaderPtr;
    (*shooters)[shooterIndex].rightLoader = rightLoaderPtr;
    (*shooters)[shooterIndex].leftLoaderId = leftLoaderIdInt;
    (*shooters)[shooterIndex].rightLoaderId = rightLoaderIdInt;
  } else {
    printf("Error: Shooter with ID %d not found\n", shooterIdInt);
  }
}

static void perform_shift_operation(Shooter_t **shooters, int shootersCount,
                                    int shooterId, const char *direction,
                                    int times, Loader_t *loaders,
                                    int loadersCount) {
  int shooterIndex =
      find_shooter_index_by_id(shooters, shootersCount, shooterId);
  if (shooterIndex == -1) {
    printf("Error: Shooter with ID %d not found\n", shooterId);
    return;
  }

  Shooter_t *shooter = &(*shooters)[shooterIndex];

  // Resolve current loader pointers from stored IDs (rebinding after reallocs)
  Loader_t *resolvedLeft = NULL;
  Loader_t *resolvedRight = NULL;
  if (shooter->leftLoaderId != -1) {
    for (int i = 0; i < loadersCount; i++) {
      if (loaders[i].id == shooter->leftLoaderId) {
        resolvedLeft = &loaders[i];
        break;
      }
    }
  }
  if (shooter->rightLoaderId != -1) {
    for (int i = 0; i < loadersCount; i++) {
      if (loaders[i].id == shooter->rightLoaderId) {
        resolvedRight = &loaders[i];
        break;
      }
    }
  }
  shooter->leftLoader = resolvedLeft;
  shooter->rightLoader = resolvedRight;

  for (int i = 0; i < times; i++) {
    if (strcmp(direction, "e") == 0) {
      // Check if left loader exists and has shapes
      if (shooter->leftLoader == NULL ||
          stack_is_empty(*(shooter->leftLoader->shapes))) {
        continue; // Skip silently if no shapes available
      }

      // If shooter has a shape, move it to right loader
      if (shooter->shootingPosition != NULL && shooter->rightLoader != NULL) {
        stack_push(*(shooter->rightLoader->shapes), shooter->shootingPosition);
      }

      shooter->shootingPosition = stack_pop(*(shooter->leftLoader->shapes));
    }
    if (strcmp(direction, "d") == 0) {
      // Check if right loader exists and has shapes
      if (shooter->rightLoader == NULL ||
          stack_is_empty(*(shooter->rightLoader->shapes))) {
        continue; // Skip silently if no shapes available
      }

      // If shooter has a shape, move it to left loader
      if (shooter->shootingPosition != NULL && shooter->leftLoader != NULL) {
        stack_push(*(shooter->leftLoader->shapes), shooter->shootingPosition);
      }

      shooter->shootingPosition = stack_pop(*(shooter->rightLoader->shapes));
    }
  }
}

static void execute_shft_command(Shooter_t **shooters, int *shootersCount,
                                 Loader_t *loaders, int *loadersCount) {
  char *shooterId = strtok(NULL, " ");
  char *leftOrRightButton = strtok(NULL, " ");
  char *timesPressed = strtok(NULL, " ");

  int shooterIdInt = atoi(shooterId);
  int timesPressedInt = atoi(timesPressed);

  perform_shift_operation(shooters, *shootersCount, shooterIdInt,
                          leftOrRightButton, timesPressedInt, loaders,
                          *loadersCount);
}

static void perform_shoot_operation(Shooter_t **shooters, int shootersCount,
                                    int shooterId, double dx, double dy,
                                    const char *annotate, Stack arena,
                                    Stack stackToFree) {
  int shooterIndex =
      find_shooter_index_by_id(shooters, shootersCount, shooterId);
  if (shooterIndex == -1) {
    printf("Error: Shooter with ID %d not found\n", shooterId);
    return;
  }

  Shooter_t *shooter = &(*shooters)[shooterIndex];

  // Check if shooter has a shape to shoot
  if (shooter->shootingPosition == NULL) {
    return; // Skip silently if no shape to shoot
  }

  double shapeXOnArena = shooter->x + dx;
  double shapeYOnArena = shooter->y + dy;

  Shape_t *shape = (Shape_t *)shooter->shootingPosition;
  ShapeType shapeType = shape->type;

  // Add shape to arena
  ShapePositionOnArena_t *shapePositionOnArena =
      malloc(sizeof(ShapePositionOnArena_t));
  if (shapePositionOnArena == NULL) {
    printf("Error: Failed to allocate memory for ShapePositionOnArena\n");
    exit(1);
  }
  shapePositionOnArena->shape = shape;
  shapePositionOnArena->x = shapeXOnArena;
  shapePositionOnArena->y = shapeYOnArena;
  shapePositionOnArena->isAnnotated = strcmp(annotate, "v") == 0;
  shapePositionOnArena->shooterX = shooter->x;
  shapePositionOnArena->shooterY = shooter->y;

  // Clear shooter shooting position
  shooter->shootingPosition = NULL;

  stack_push(arena, (void *)shapePositionOnArena);
  stack_push(stackToFree, (void *)shapePositionOnArena);
}

static void execute_dsp_command(Shooter_t **shooters, int *shootersCount,
                                Stack arena, Stack stackToFree) {
  char *shooterId = strtok(NULL, " ");
  char *dx = strtok(NULL, " ");
  char *dy = strtok(NULL, " ");
  char *annotateDimensions = strtok(NULL, " "); // this can be "v" or "i"

  int shooterIdInt = atoi(shooterId);
  double dxDouble = atof(dx);
  double dyDouble = atof(dy);

  perform_shoot_operation(shooters, *shootersCount, shooterIdInt, dxDouble,
                          dyDouble, annotateDimensions, arena, stackToFree);
}

static void execute_rjd_command(Shooter_t **shooters, int *shootersCount,
                                Stack stackToFree, Stack arena,
                                Loader_t *loaders, int *loadersCount) {
  char *shooterId = strtok(NULL, " ");
  char *leftOrRightButton = strtok(NULL, " ");
  char *dx = strtok(NULL, " ");
  char *dy = strtok(NULL, " ");
  char *incrementX = strtok(NULL, " ");
  char *incrementY = strtok(NULL, " ");

  int shooterIdInt = atoi(shooterId);
  double dxDouble = atof(dx);
  double dyDouble = atof(dy);
  double incrementXDouble = atof(incrementX);
  double incrementYDouble = atof(incrementY);

  int shooterIndex =
      find_shooter_index_by_id(shooters, *shootersCount, shooterIdInt);
  if (shooterIndex == -1) {
    printf("Error: Shooter with ID %d not found\n", shooterIdInt);
    return;
  }

  Shooter_t *shooter = &(*shooters)[shooterIndex];
  Loader_t *loader = NULL;
  // Rebind current pointers based on IDs in case loaders was reallocated
  if (strcmp(leftOrRightButton, "e") == 0) {
    // left side
    int targetId = (*shooters)[shooterIndex].leftLoaderId;
    if (targetId != -1) {
      for (int i = 0; i < *loadersCount; i++) {
        if (loaders[i].id == targetId) {
          (*shooters)[shooterIndex].leftLoader = &loaders[i];
          break;
        }
      }
    }
  } else if (strcmp(leftOrRightButton, "d") == 0) {
    int targetId = (*shooters)[shooterIndex].rightLoaderId;
    if (targetId != -1) {
      for (int i = 0; i < *loadersCount; i++) {
        if (loaders[i].id == targetId) {
          (*shooters)[shooterIndex].rightLoader = &loaders[i];
          break;
        }
      }
    }
  }
  // Select the same side that perform_shift_operation will consume from
  if (strcmp(leftOrRightButton, "e") == 0) {
    loader = shooter->leftLoader;
  } else if (strcmp(leftOrRightButton, "d") == 0) {
    loader = shooter->rightLoader;
  } else {
    printf("Error: Invalid button (should be 'e' or 'd')\n");
    return;
  }

  if (loader == NULL || loader->shapes == NULL) {
    printf("Error: Loader not found or shapes stack is NULL\n");
    return;
  }

  int times = 1;

  // Loop until loader is empty
  while (!stack_is_empty(*(loader->shapes))) {
    perform_shift_operation(shooters, *shootersCount, shooterIdInt,
                            leftOrRightButton, 1, loaders, *loadersCount);
    perform_shoot_operation(shooters, *shootersCount, shooterIdInt,
                            times * incrementXDouble + dxDouble,
                            times * incrementYDouble + dyDouble, "i", arena,
                            stackToFree);
    times++;
  }
}

void execute_calc_command(Stack arena, Ground ground) {

  // FILE *svgFile = fopen("output.svg", "w");
  // if (svgFile == NULL) {
  //   printf("Error: Failed to open output.svg\n");
  //   exit(1);
  // }
  // fprintf(svgFile, "<svg width=\"100%%\" height=\"100%%\" viewBox=\"0 0 100 "
  //                  "100\" xmlns=\"http://www.w3.org/2000/svg\">\n");

  // We need to process in launch order (oldest to newest). Arena is a stack
  // (LIFO), so first reverse into a temporary stack to get FIFO order when
  // popping.
  Stack temp = stack_create();
  while (!stack_is_empty(arena)) {
    stack_push(temp, stack_pop(arena));
  }

  // Now process adjacent pairs I (older) and J (I+1 newer).
  while (!stack_is_empty(temp)) {
    ShapePositionOnArena_t *I = (ShapePositionOnArena_t *)stack_pop(temp);
    if (stack_is_empty(temp)) {
      // No pair for I, return to ground at its arena position
      Shape_t *Ipos = clone_with_position(I->shape, I->x, I->y);
      if (Ipos != NULL) {
        queue_enqueue(get_ground_queue(ground), Ipos);
      }
      continue;
    }
    ShapePositionOnArena_t *J = (ShapePositionOnArena_t *)stack_pop(temp);

    bool overlap = shapes_overlap(I, J);
    if (overlap) {
      double areaI = shape_area(I->shape->type, I->shape->data);
      double areaJ = shape_area(J->shape->type, J->shape->data);

      if (areaI < areaJ) {
        // I is destroyed; J goes back to ground at its arena position
        Shape_t *Jpos = clone_with_position(J->shape, J->x, J->y);
        if (Jpos != NULL) {
          queue_enqueue(get_ground_queue(ground), Jpos);
        }
      } else if (areaI > areaJ) {
        // I changes border color of J to fill color of I, if applicable
        const char *fillColorI = NULL;
        switch (I->shape->type) {
        case CIRCLE:
          fillColorI = circle_get_fill_color((Circle)I->shape->data);
          break;
        case RECTANGLE:
          fillColorI = rectangle_get_fill_color((Rectangle)I->shape->data);
          break;
        case TEXT:
          fillColorI = text_get_fill_color((Text)I->shape->data);
          break;
        case LINE:
        case TEXT_STYLE:
          fillColorI = NULL;
          break;
        }

        // Prepare J' with new border and positioned at J
        Shape_t *JprimePos = NULL;
        if (fillColorI != NULL) {
          JprimePos = clone_with_border_color_at_position(J->shape, fillColorI,
                                                          J->x, J->y);
        } else {
          JprimePos = clone_with_position(J->shape, J->x, J->y);
        }

        // Both return to ground in original relative order (I, then J') at
        // their positions
        Shape_t *Ipos = clone_with_position(I->shape, I->x, I->y);
        if (Ipos != NULL) {
          queue_enqueue(get_ground_queue(ground), Ipos);
        }
        if (JprimePos != NULL) {
          queue_enqueue(get_ground_queue(ground), JprimePos);
        }

        // Clone I swapping border and fill (only if applicable), at I position
        Shape_t *IclonePos =
            clone_with_swapped_colors_at_position(I->shape, I->x, I->y);
        if (IclonePos != NULL) {
          queue_enqueue(get_ground_queue(ground), IclonePos);
        }
      } else {
        // Equal areas: both return unchanged at their positions
        Shape_t *Ipos = clone_with_position(I->shape, I->x, I->y);
        Shape_t *Jpos = clone_with_position(J->shape, J->x, J->y);
        if (Ipos != NULL) {
          queue_enqueue(get_ground_queue(ground), Ipos);
        }
        if (Jpos != NULL) {
          queue_enqueue(get_ground_queue(ground), Jpos);
        }
      }
    } else {
      // No overlap: both return unchanged in the same relative order, placed at
      // their positions
      Shape_t *Ipos = clone_with_position(I->shape, I->x, I->y);
      Shape_t *Jpos = clone_with_position(J->shape, J->x, J->y);
      if (Ipos != NULL) {
        queue_enqueue(get_ground_queue(ground), Ipos);
      }
      if (Jpos != NULL) {
        queue_enqueue(get_ground_queue(ground), Jpos);
      }
    }
  }
}

static int find_shooter_index_by_id(Shooter_t **shooters, int shootersCount,
                                    int id) {
  for (int i = 0; i < shootersCount; i++) {
    if ((*shooters)[i].id == id) {
      return i;
    }
  }
  return -1;
}

// =====================
// Helpers implementation
// =====================

static Shape_t *make_shape_wrapper(ShapeType type, void *data) {
  Shape_t *s = (Shape_t *)malloc(sizeof(Shape_t));
  if (s == NULL) {
    printf("Error: Failed to allocate shape wrapper\n");
    exit(1);
  }
  s->type = type;
  s->data = data;
  return s;
}

static double shape_area(ShapeType type, void *shapeData) {
  switch (type) {
  case CIRCLE: {
    double r = circle_get_radius((Circle)shapeData);
    return 3.141592653589793 * r * r;
  }
  case RECTANGLE: {
    double w = rectangle_get_width((Rectangle)shapeData);
    double h = rectangle_get_height((Rectangle)shapeData);
    return w * h;
  }
  case LINE: {
    double x1 = line_get_x1((Line)shapeData);
    double y1 = line_get_y1((Line)shapeData);
    double x2 = line_get_x2((Line)shapeData);
    double y2 = line_get_y2((Line)shapeData);
    double dx = x2 - x1;
    double dy = y2 - y1;
    double len = (dx * dx + dy * dy) > 0.0 ? sqrt(dx * dx + dy * dy) : 0.0;
    return 2.0 * len;
  }
  case TEXT: {
    const char *txt = text_get_text((Text)shapeData);
    int len = (int)strlen(txt);
    return 20.0 * (double)len;
  }
  case TEXT_STYLE:
    return 0.0;
  }
  return 0.0;
}

static Aabb make_aabb_for_shape_on_arena(const ShapePositionOnArena_t *s) {
  Aabb box;
  switch (s->shape->type) {
  case CIRCLE: {
    double r = circle_get_radius((Circle)s->shape->data);
    box.minX = s->x - r;
    box.maxX = s->x + r;
    box.minY = s->y - r;
    box.maxY = s->y + r;
    break;
  }
  case RECTANGLE: {
    double w = rectangle_get_width((Rectangle)s->shape->data);
    double h = rectangle_get_height((Rectangle)s->shape->data);
    box.minX = s->x;
    box.minY = s->y;
    box.maxX = s->x + w;
    box.maxY = s->y + h;
    break;
  }
  case TEXT: {
    const char *txt = text_get_text((Text)s->shape->data);
    int len = (int)strlen(txt);
    double height = 20.0;       // consistent with area rule
    double width = (double)len; // width*height = 20*len
    box.minX = s->x;
    box.minY = s->y;
    box.maxX = s->x + width;
    box.maxY = s->y + height;
    break;
  }
  case LINE: {
    double x1 = line_get_x1((Line)s->shape->data);
    double y1 = line_get_y1((Line)s->shape->data);
    double x2 = line_get_x2((Line)s->shape->data);
    double y2 = line_get_y2((Line)s->shape->data);
    double dx = x2 - x1;
    double dy = y2 - y1;
    double minLocalX = (dx < 0.0) ? dx : 0.0;
    double maxLocalX = (dx > 0.0) ? dx : 0.0;
    double minLocalY = (dy < 0.0) ? dy : 0.0;
    double maxLocalY = (dy > 0.0) ? dy : 0.0;
    // thickness 2.0 => inflate by 1 on each side
    box.minX = s->x + minLocalX - 1.0;
    box.maxX = s->x + maxLocalX + 1.0;
    box.minY = s->y + minLocalY - 1.0;
    box.maxY = s->y + maxLocalY + 1.0;
    break;
  }
  case TEXT_STYLE: {
    // No extent, treat as empty box
    box.minX = box.maxX = s->x;
    box.minY = box.maxY = s->y;
    break;
  }
  }
  return box;
}

static bool aabb_overlap(Aabb a, Aabb b) {
  if (a.maxX < b.minX)
    return false;
  if (b.maxX < a.minX)
    return false;
  if (a.maxY < b.minY)
    return false;
  if (b.maxY < a.minY)
    return false;
  return true;
}

static bool shapes_overlap(const ShapePositionOnArena_t *a,
                           const ShapePositionOnArena_t *b) {
  Aabb aa = make_aabb_for_shape_on_arena(a);
  Aabb bb = make_aabb_for_shape_on_arena(b);
  return aabb_overlap(aa, bb);
}

static Shape_t *clone_with_border_color(Shape_t *src,
                                        const char *newBorderColor) {
  switch (src->type) {
  case CIRCLE: {
    Circle c = (Circle)src->data;
    int id = circle_get_id(c);
    double x = 0.0; // position handled by arena, keep model values
    double y = 0.0;
    // Preserve original geometry from getters
    x = circle_get_x(c);
    y = circle_get_y(c);
    double r = circle_get_radius(c);
    const char *fill = circle_get_fill_color(c);
    Circle nc = circle_create(id, x, y, r, newBorderColor, fill);
    return make_shape_wrapper(CIRCLE, nc);
  }
  case RECTANGLE: {
    Rectangle r = (Rectangle)src->data;
    int id = rectangle_get_id(r);
    double x = rectangle_get_x(r);
    double y = rectangle_get_y(r);
    double w = rectangle_get_width(r);
    double h = rectangle_get_height(r);
    const char *fill = rectangle_get_fill_color(r);
    Rectangle nr = rectangle_create(id, x, y, w, h, newBorderColor, fill);
    return make_shape_wrapper(RECTANGLE, nr);
  }
  case TEXT: {
    Text t = (Text)src->data;
    int id = text_get_id(t);
    double x = text_get_x(t);
    double y = text_get_y(t);
    const char *fill = text_get_fill_color(t);
    char anchor = text_get_anchor(t);
    const char *txt = text_get_text(t);
    Text nt = text_create(id, x, y, newBorderColor, fill, anchor, txt);
    return make_shape_wrapper(TEXT, nt);
  }
  case LINE: {
    Line l = (Line)src->data;
    int id = line_get_id(l);
    double x1 = line_get_x1(l);
    double y1 = line_get_y1(l);
    double x2 = line_get_x2(l);
    double y2 = line_get_y2(l);
    Line nl = line_create(id, x1, y1, x2, y2, newBorderColor);
    return make_shape_wrapper(LINE, nl);
  }
  case TEXT_STYLE:
    return NULL;
  }
  return NULL;
}

static Shape_t *clone_with_swapped_colors(Shape_t *src) {
  switch (src->type) {
  case CIRCLE: {
    Circle c = (Circle)src->data;
    int id = circle_get_id(c);
    double x = circle_get_x(c);
    double y = circle_get_y(c);
    double r = circle_get_radius(c);
    const char *border = circle_get_border_color(c);
    const char *fill = circle_get_fill_color(c);
    Circle nc = circle_create(id, x, y, r, fill, border);
    return make_shape_wrapper(CIRCLE, nc);
  }
  case RECTANGLE: {
    Rectangle r = (Rectangle)src->data;
    int id = rectangle_get_id(r);
    double x = rectangle_get_x(r);
    double y = rectangle_get_y(r);
    double w = rectangle_get_width(r);
    double h = rectangle_get_height(r);
    const char *border = rectangle_get_border_color(r);
    const char *fill = rectangle_get_fill_color(r);
    Rectangle nr = rectangle_create(id, x, y, w, h, fill, border);
    return make_shape_wrapper(RECTANGLE, nr);
  }
  case TEXT: {
    Text t = (Text)src->data;
    int id = text_get_id(t);
    double x = text_get_x(t);
    double y = text_get_y(t);
    const char *border = text_get_border_color(t);
    const char *fill = text_get_fill_color(t);
    char anchor = text_get_anchor(t);
    const char *txt = text_get_text(t);
    Text nt = text_create(id, x, y, fill, border, anchor, txt);
    return make_shape_wrapper(TEXT, nt);
  }
  case LINE:
  case TEXT_STYLE:
    // No fill color to swap
    return NULL;
  }
  return NULL;
}

// =====================
// Positioning helpers
// =====================

static void destroy_shape(Shape_t *s) {
  if (s == NULL)
    return;
  switch (s->type) {
  case CIRCLE:
    circle_destroy((Circle)s->data);
    break;
  case RECTANGLE:
    rectangle_destroy((Rectangle)s->data);
    break;
  case LINE:
    line_destroy((Line)s->data);
    break;
  case TEXT:
    text_destroy((Text)s->data);
    break;
  case TEXT_STYLE:
    // nothing allocated
    break;
  }
  free(s);
}

static Shape_t *clone_with_position(Shape_t *src, double x, double y) {
  if (src == NULL)
    return NULL;
  switch (src->type) {
  case CIRCLE: {
    Circle c = (Circle)src->data;
    int id = circle_get_id(c);
    double r = circle_get_radius(c);
    const char *border = circle_get_border_color(c);
    const char *fill = circle_get_fill_color(c);
    Circle nc = circle_create(id, x, y, r, border, fill);
    return make_shape_wrapper(CIRCLE, nc);
  }
  case RECTANGLE: {
    Rectangle r = (Rectangle)src->data;
    int id = rectangle_get_id(r);
    double w = rectangle_get_width(r);
    double h = rectangle_get_height(r);
    const char *border = rectangle_get_border_color(r);
    const char *fill = rectangle_get_fill_color(r);
    Rectangle nr = rectangle_create(id, x, y, w, h, border, fill);
    return make_shape_wrapper(RECTANGLE, nr);
  }
  case TEXT: {
    Text t = (Text)src->data;
    int id = text_get_id(t);
    const char *border = text_get_border_color(t);
    const char *fill = text_get_fill_color(t);
    char anchor = text_get_anchor(t);
    const char *txt = text_get_text(t);
    Text nt = text_create(id, x, y, border, fill, anchor, txt);
    return make_shape_wrapper(TEXT, nt);
  }
  case LINE: {
    Line l = (Line)src->data;
    int id = line_get_id(l);
    double dx = line_get_x2(l) - line_get_x1(l);
    double dy = line_get_y2(l) - line_get_y1(l);
    Line nl = line_create(id, x, y, x + dx, y + dy, line_get_color(l));
    return make_shape_wrapper(LINE, nl);
  }
  case TEXT_STYLE:
    return NULL;
  }
  return NULL;
}

static Shape_t *clone_with_border_color_at_position(Shape_t *src,
                                                    const char *newBorderColor,
                                                    double x, double y) {
  if (src == NULL)
    return NULL;
  switch (src->type) {
  case CIRCLE: {
    Circle c = (Circle)src->data;
    int id = circle_get_id(c);
    double r = circle_get_radius(c);
    const char *fill = circle_get_fill_color(c);
    Circle nc = circle_create(id, x, y, r, newBorderColor, fill);
    return make_shape_wrapper(CIRCLE, nc);
  }
  case RECTANGLE: {
    Rectangle r = (Rectangle)src->data;
    int id = rectangle_get_id(r);
    double w = rectangle_get_width(r);
    double h = rectangle_get_height(r);
    const char *fill = rectangle_get_fill_color(r);
    Rectangle nr = rectangle_create(id, x, y, w, h, newBorderColor, fill);
    return make_shape_wrapper(RECTANGLE, nr);
  }
  case TEXT: {
    Text t = (Text)src->data;
    int id = text_get_id(t);
    const char *fill = text_get_fill_color(t);
    char anchor = text_get_anchor(t);
    const char *txt = text_get_text(t);
    Text nt = text_create(id, x, y, newBorderColor, fill, anchor, txt);
    return make_shape_wrapper(TEXT, nt);
  }
  case LINE: {
    Line l = (Line)src->data;
    int id = line_get_id(l);
    double dx = line_get_x2(l) - line_get_x1(l);
    double dy = line_get_y2(l) - line_get_y1(l);
    Line nl = line_create(id, x, y, x + dx, y + dy, newBorderColor);
    return make_shape_wrapper(LINE, nl);
  }
  case TEXT_STYLE:
    return NULL;
  }
  return NULL;
}

static Shape_t *clone_with_swapped_colors_at_position(Shape_t *src, double x,
                                                      double y) {
  if (src == NULL)
    return NULL;
  switch (src->type) {
  case CIRCLE: {
    Circle c = (Circle)src->data;
    int id = circle_get_id(c);
    double r = circle_get_radius(c);
    const char *border = circle_get_border_color(c);
    const char *fill = circle_get_fill_color(c);
    Circle nc = circle_create(id, x, y, r, fill, border);
    return make_shape_wrapper(CIRCLE, nc);
  }
  case RECTANGLE: {
    Rectangle r = (Rectangle)src->data;
    int id = rectangle_get_id(r);
    double w = rectangle_get_width(r);
    double h = rectangle_get_height(r);
    const char *border = rectangle_get_border_color(r);
    const char *fill = rectangle_get_fill_color(r);
    Rectangle nr = rectangle_create(id, x, y, w, h, fill, border);
    return make_shape_wrapper(RECTANGLE, nr);
  }
  case TEXT: {
    Text t = (Text)src->data;
    int id = text_get_id(t);
    const char *border = text_get_border_color(t);
    const char *fill = text_get_fill_color(t);
    char anchor = text_get_anchor(t);
    const char *txt = text_get_text(t);
    Text nt = text_create(id, x, y, fill, border, anchor, txt);
    return make_shape_wrapper(TEXT, nt);
  }
  case LINE:
  case TEXT_STYLE:
    return NULL;
  }
  return NULL;
}

// =====================
// SVG writer implementation
// =====================
static void write_qry_result_svg(FileData qryFileData, FileData geoFileData,
                                 Ground ground, Stack arena,
                                 const char *output_path) {
  const char *geo_name_src = get_file_name(geoFileData);
  const char *qry_name_src = get_file_name(qryFileData);
  size_t geo_len = strlen(geo_name_src);
  size_t qry_len = strlen(qry_name_src);

  char *geo_base = malloc(geo_len + 1);
  char *qry_base = malloc(qry_len + 1);
  if (geo_base == NULL || qry_base == NULL) {
    printf("Error: Memory allocation failed for file name\n");
    free(geo_base);
    free(qry_base);
    return;
  }
  strcpy(geo_base, geo_name_src);
  strcpy(qry_base, qry_name_src);
  strtok(geo_base, ".");
  strtok(qry_base, ".");

  // geoBase-qryBase.svg
  size_t path_len = strlen(output_path);
  size_t processed_name_len = strlen(geo_base) + 1 + strlen(qry_base);
  size_t total_len = path_len + 1 + processed_name_len + 4 + 1;
  char *output_path_with_file = malloc(total_len);
  if (output_path_with_file == NULL) {
    printf("Error: Memory allocation failed\n");
    free(geo_base);
    free(qry_base);
    return;
  }

  int result = snprintf(output_path_with_file, total_len, "%s/%s-%s.svg",
                        output_path, geo_base, qry_base);
  if (result < 0 || (size_t)result >= total_len) {
    printf("Error: Path construction failed\n");
    free(output_path_with_file);
    free(geo_base);
    free(qry_base);
    return;
  }

  FILE *file = fopen(output_path_with_file, "w");
  if (file == NULL) {
    printf("Error: Failed to open file: %s\n", output_path_with_file);
    free(output_path_with_file);
    free(geo_base);
    free(qry_base);
    return;
  }

  fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  fprintf(file, "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 10000 "
                "10000\">\n");

  // Render remaining shapes from Ground without destroying the queue
  Queue groundQueue = get_ground_queue(ground);
  Queue tempQueue = queue_create();
  while (!queue_is_empty(groundQueue)) {
    Shape_t *shape = (Shape_t *)queue_dequeue(groundQueue);
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
        const char *text_anchor = "start";
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
    queue_enqueue(tempQueue, shape);
  }
  // restore ground queue
  while (!queue_is_empty(tempQueue)) {
    queue_enqueue(groundQueue, queue_dequeue(tempQueue));
  }
  queue_destroy(tempQueue);

  // Render annotations from arena without destroying the stack
  Stack tempStack = stack_create();
  while (!stack_is_empty(arena)) {
    ShapePositionOnArena_t *s = (ShapePositionOnArena_t *)stack_pop(arena);
    if (s != NULL && s->isAnnotated) {
      // dashed line from shooter to landed position
      fprintf(file,
              "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='red' "
              "stroke-dasharray='4,2' stroke-width='1'/>\n",
              s->shooterX, s->shooterY, s->x, s->y);
      // small circle marker at landed position
      fprintf(file,
              "<circle cx='%.2f' cy='%.2f' r='3' fill='none' stroke='red' "
              "stroke-width='1'/>\n",
              s->x, s->y);

      // dimension guides (horizontal then vertical) and labels (dx, dy)
      double dx = s->x - s->shooterX;
      double dy = s->y - s->shooterY;
      double midHx = s->shooterX + dx * 0.5;
      double midHy = s->shooterY;
      double midVx = s->x;
      double midVy = s->shooterY + dy * 0.5;

      // horizontal guide
      fprintf(file,
              "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='purple' "
              "stroke-dasharray='2,2' stroke-width='0.8'/>\n",
              s->shooterX, s->shooterY, s->x, s->shooterY);
      // vertical guide
      fprintf(file,
              "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='purple' "
              "stroke-dasharray='2,2' stroke-width='0.8'/>\n",
              s->x, s->shooterY, s->x, s->y);

      // dx label above horizontal guide
      fprintf(file,
              "<text x='%.2f' y='%.2f' fill='purple' font-size='12' "
              "text-anchor='middle'>%.2f</text>\n",
              midHx, midHy - 5.0, dx);

      // dy label rotated near vertical guide
      fprintf(file,
              "<text x='%.2f' y='%.2f' fill='purple' font-size='12' "
              "text-anchor='middle' transform='rotate(-90 %.2f "
              "%.2f)'>%.2f</text>\n",
              midVx + 10.0, midVy, midVx + 10.0, midVy, dy);
    }
    stack_push(tempStack, s);
  }
  // restore arena stack (original order)
  while (!stack_is_empty(tempStack)) {
    stack_push(arena, stack_pop(tempStack));
  }
  stack_destroy(tempStack);

  fprintf(file, "</svg>\n");
  fclose(file);
  free(output_path_with_file);
  free(geo_base);
  free(qry_base);
}