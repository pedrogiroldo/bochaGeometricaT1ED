#include "geo_handler.h"
#include "../commons/queue/queue.h"
#include <stdio.h>
#include <string.h>

void execute_geo_commands(Queue lines) {
  while (!queue_is_empty(lines)) {
    char *line = (char *)queue_dequeue(lines);
    char *command = strtok(line, " ");

    // Circle command: c i x y r corb corp
    if (strcmp(command, "c") == 0) {
      char *identifier = strtok(NULL, " ");
      char *posX = strtok(NULL, " ");
      char *posY = strtok(NULL, " ");
      char *radius = strtok(NULL, " ");
      char *borderColor = strtok(NULL, " ");
      char *fillColor = strtok(NULL, " ");

      printf("Circle: id=%s, center=(%s,%s), radius=%s, border=%s, fill=%s\n",
             identifier, posX, posY, radius, borderColor, fillColor);
    }
    // Rectangle command: r i x y w h corb corp
    else if (strcmp(command, "r") == 0) {
      char *identifier = strtok(NULL, " ");
      char *posX = strtok(NULL, " ");
      char *posY = strtok(NULL, " ");
      char *width = strtok(NULL, " ");
      char *height = strtok(NULL, " ");
      char *borderColor = strtok(NULL, " ");
      char *fillColor = strtok(NULL, " ");

      printf("Rectangle: id=%s, anchor=(%s,%s), width=%s, height=%s, "
             "border=%s, fill=%s\n",
             identifier, posX, posY, width, height, borderColor, fillColor);
    }
    // Line command: l i x1 y1 x2 y2 cor
    else if (strcmp(command, "l") == 0) {
      char *identifier = strtok(NULL, " ");
      char *x1 = strtok(NULL, " ");
      char *y1 = strtok(NULL, " ");
      char *x2 = strtok(NULL, " ");
      char *y2 = strtok(NULL, " ");
      char *color = strtok(NULL, " ");

      printf("Line: id=%s, from=(%s,%s) to=(%s,%s), color=%s\n", identifier, x1,
             y1, x2, y2, color);
    }
    // Text command: t i x y corb corp a txto
    else if (strcmp(command, "t") == 0) {
      char *identifier = strtok(NULL, " ");
      char *posX = strtok(NULL, " ");
      char *posY = strtok(NULL, " ");
      char *borderColor = strtok(NULL, " ");
      char *fillColor = strtok(NULL, " ");
      char *anchor = strtok(NULL, " ");
      char *text = strtok(NULL, ""); // Get remaining text until end of line

      printf("Text: id=%s, pos=(%s,%s), border=%s, fill=%s, anchor=%s, "
             "text='%s'\n",
             identifier, posX, posY, borderColor, fillColor, anchor, text);
    }
    // Text style command: ts fFamily fWeight fSize
    else if (strcmp(command, "ts") == 0) {
      char *fontFamily = strtok(NULL, " ");
      char *fontWeight = strtok(NULL, " ");
      char *fontSize = strtok(NULL, " ");

      printf("Text Style: family=%s, weight=%s, size=%s\n", fontFamily,
             fontWeight, fontSize);
    } else {
      printf("Unknown command: %s\n", command);
    }
  }
}
