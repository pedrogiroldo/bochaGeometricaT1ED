#include "qry_handler.h"
#include "../commons/stack/stack.h"
#include "../geo_handler/geo_handler.h"
#include "../shapes/shapes.h"
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
                                 Shooter_t **shooters, int *shootersCount);
static void perform_shift_operation(Shooter_t **shooters, int shootersCount,
                                    int shooterId, const char *direction,
                                    int times);
static void perform_shoot_operation(Shooter_t **shooters, int shootersCount,
                                    int shooterId, double dx, double dy,
                                    const char *annotate, Stack arena,
                                    Stack stackToFree);
static void execute_shft_command(Shooter_t **shooters, int *shootersCount);
static void execute_dsp_command(Shooter_t **shooters, int *shootersCount,
                                Stack arena, Stack stackToFree);
static void execute_rjd_command(Shooter_t **shooters, int *shootersCount,
                                Stack stackToFree, Stack arena);
static void execute_calc_command();
static int find_shooter_index_by_id(Shooter_t **shooters, int shootersCount,
                                    int id);

void execute_qry_commands(FileData fileData, Ground ground,
                          const char *output_path, const char *command_suffix) {

  Qry_t *qry = malloc(sizeof(Qry_t));
  if (qry == NULL) {
    printf("Error: Failed to allocate memory for Qry\n");
    exit(1);
  }
  qry->arena = stack_create();
  qry->stackToFree = stack_create();

  // Add qry to stackToFree for cleanup
  stack_push(qry->stackToFree, qry);

  Shooter_t **shooters = NULL;
  int shootersCount = 0;
  Loader_t **loaders = NULL;
  int loadersCount = 0;

  while (!queue_is_empty(get_file_lines_queue(fileData))) {
    char *line = (char *)queue_dequeue(get_file_lines_queue(fileData));
    char *command = strtok(line, " ");

    if (strcmp(command, "pd") == 0) {
      execute_pd_command(shooters, &shootersCount, qry->stackToFree);
    } else if (strcmp(command, "lc") == 0) {
      execute_lc_command(loaders, &loadersCount, ground, qry->stackToFree);
    } else if (strcmp(command, "atch") == 0) {
      execute_atch_command(loaders, &loadersCount, shooters, &shootersCount);
    } else if (strcmp(command, "shft") == 0) {
      execute_shft_command(fileData, ground);
    } else if (strcmp(command, "dsp") == 0) {
      execute_dsp_command(shooters, &shootersCount, qry->arena,
                          qry->stackToFree);
    } else if (strcmp(command, "rjd") == 0) {
      execute_rjd_command(shooters, &shootersCount, qry->stackToFree,
                          qry->arena);
    } else if (strcmp(command, "calc") == 0) {
      // execute_calc_command(fileData, ground); // TODO: Implement this
      // function
    } else
      printf("Unknown command: %s\n", command);
  }
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
                                                .leftLoader = NULL};
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
                                 Shooter_t **shooters, int *shootersCount) {
  char *shooterId = strtok(NULL, " ");
  char *leftLoaderId = strtok(NULL, " ");
  char *rightLoaderId = strtok(NULL, " ");

  int shooterIdInt = atoi(shooterId);
  int leftLoaderIdInt = atoi(leftLoaderId);
  int rightLoaderIdInt = atoi(rightLoaderId);

  int shooterIndex =
      find_shooter_index_by_id(shooters, *shootersCount, shooterIdInt);
  if (shooterIndex != -1) {
    // Para cada lado, percorre os loaders até encontrar o id correspondente
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

    // Verifica se os loaders foram encontrados
    if (leftLoaderPtr == NULL) {
      printf("Error: Loader with ID %d not found\n", leftLoaderIdInt);
    }
    if (rightLoaderPtr == NULL) {
      printf("Error: Loader with ID %d not found\n", rightLoaderIdInt);
    }

    (*shooters)[shooterIndex].leftLoader = leftLoaderPtr;
    (*shooters)[shooterIndex].rightLoader = rightLoaderPtr;
  } else {
    printf("Error: Shooter with ID %d not found\n", shooterIdInt);
  }
}

static void perform_shift_operation(Shooter_t **shooters, int shootersCount,
                                    int shooterId, const char *direction,
                                    int times) {
  int shooterIndex =
      find_shooter_index_by_id(shooters, shootersCount, shooterId);
  if (shooterIndex == -1) {
    printf("Error: Shooter with ID %d not found\n", shooterId);
    return;
  }

  Shooter_t *shooter = &(*shooters)[shooterIndex];

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

static void execute_shft_command(Shooter_t **shooters, int *shootersCount) {
  char *shooterId = strtok(NULL, " ");
  char *leftOrRightButton = strtok(NULL, " ");
  char *timesPressed = strtok(NULL, " ");

  int shooterIdInt = atoi(shooterId);
  int timesPressedInt = atoi(timesPressed);

  perform_shift_operation(shooters, *shootersCount, shooterIdInt,
                          leftOrRightButton, timesPressedInt);
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
                                Stack stackToFree, Stack arena) {
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
  if (strcmp(leftOrRightButton, "e") == 0) {
    loader = shooter->rightLoader;
  } else if (strcmp(leftOrRightButton, "d") == 0) {
    loader = shooter->leftLoader;
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
  while (!stack_is_empty(loader->shapes)) {
    perform_shift_operation(shooters, *shootersCount, shooterIdInt,
                            leftOrRightButton, 1);
    perform_shoot_operation(shooters, *shootersCount, shooterIdInt,
                            times * incrementXDouble + dxDouble,
                            times * incrementYDouble + dyDouble, "i", arena,
                            stackToFree);
    times++;
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