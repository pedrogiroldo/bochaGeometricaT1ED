#include "qry_handler.h"
#include "../shapes/shapes.h"

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
  Loader_t loaders[];
} Shooter_t;

void execute_qry_commands(FileData fileData, Ground ground,
                          const char *output_path, const char *command_suffix) {

}
