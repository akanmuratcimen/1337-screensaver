#ifndef TEXTURE_H
#define TEXTURE_H

struct texture {
  unsigned int id;
  int width;
  int height;
};

struct texture
load_texture(
  const char *file_name
);

void
draw_texture(
  struct texture texture,
  const char c,
  float x,
  float y
);

#endif
