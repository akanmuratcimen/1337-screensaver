#ifndef CORE_H
#define CORE_H

#include <stdbool.h>

struct texture {
  unsigned int id;
  int width;
  int height;
};

struct texture_offset {
  float x;
  float width;
};

struct texture
load_texture(
  const char *file_name
);

struct vector2 {
  float x;
  float y;
};

struct cell {
  char value;
  int column;
  int row;
  struct vector2 position;
  float pause_position;
  bool is_highlighting;
  bool is_highlighted;
};

struct column {
  float speed;
  bool is_highlighting;
  int highlighting_frame_counter;
  float top_position;
  int highlighting_row_index;
};


#endif
