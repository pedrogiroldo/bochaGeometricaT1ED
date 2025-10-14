#include "qry_handler.h"
#include "../commons/stack/stack.h"
#include "../shapes/shapes.h"
#include <stdio.h>
#include <string.h>

typedef struct {
  ShapeType type;
  void *data;
} Shape_t;

typedef struct {
  int id;
  Shape_t shapes[];
} Loader_t;

typedef struct {
  int id;
  double x;
  double y;
  Shape_t *shooterShape;
  Loader_t **loaders;
} Shooter_t;

typedef struct {
  Stack stackToFree;
} Qry_t;

// private functions
static void execute_pd_command(Shooter_t **shooters, int *shootersCount);
static void execute_lc_command(Loader_t **loaders);
static void execute_atch_command(FileData fileData, Ground ground);
static void execute_shft_command(FileData fileData, Ground ground);
static void execute_dsp_command(FileData fileData, Ground ground);
static void execute_rjd_command(FileData fileData, Ground ground);
static void execute_calc_command(FileData fileData, Ground ground);

void execute_qry_commands(FileData fileData, Ground ground,
                          const char *output_path, const char *command_suffix) {

  Shooter_t **shooters;
  int shootersCount = 0;

  while (!queue_is_empty(get_file_lines_queue(fileData))) {
    char *line = (char *)queue_dequeue(get_file_lines_queue(fileData));
    char *command = strtok(line, " ");

    if (strcmp(command, "pd") == 0) {
      execute_pd_command(shooters, &shootersCount);
    } else if (strcmp(command, "lc") == 0) {
      execute_lc_command(shooters);
    } else if (strcmp(command, "atch") == 0) {
      execute_atch_command(fileData, ground);
    } else if (strcmp(command, "shft") == 0) {
      execute_shft_command(fileData, ground);
    } else if (strcmp(command, "dsp") == 0) {
      execute_dsp_command(fileData, ground);
    } else if (strcmp(command, "rjd") == 0) {
    } else if (strcmp(command, "calc") == 0) {
      execute_calc_command(fileData, ground);
    } else
      printf("Unknown command: %s\n", command);
  }
}

/*
==========================
Private functions
==========================
*/

static void execute_pd_command(Shooter_t **shooters, int *shootersCount) {
  char *identifier = strtok(NULL, " ");
  char *posX = strtok(NULL, " ");
  char *posY = strtok(NULL, " ");

  *shootersCount += 1;

  *shooters = realloc(*shooters, *shootersCount * sizeof(Shooter_t));
  if (*shooters == NULL) {
    printf("Error: Failed to allocate memory for Shooters\n");
    exit(1);
  }
  (*shooters)[*shootersCount - 1] = (Shooter_t){.id = atoi(identifier),
                                                .x = atof(posX),
                                                .y = atof(posY),
                                                .shooterShape = NULL,
                                                .loaders = NULL};
}
