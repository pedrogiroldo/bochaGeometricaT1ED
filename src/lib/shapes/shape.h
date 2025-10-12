enum ShapeType { CIRCLE, RECTANGLE, LINE, TEXT, TEXT_STYLE };

typedef enum ShapeType ShapeType;

typedef struct {
  ShapeType type;
  void *data;
} Shape_t;
