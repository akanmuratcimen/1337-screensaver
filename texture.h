#ifndef TEXTURE_H
#define TEXTURE_H

#include "core.h"

void
draw_rectangle(
  const float x,
  const float y,
  const float w,
  const float h,
  struct window_size window_size
);

void
draw_bulk(
  const int column_count,
  const int row_count,
  struct cell cells[column_count][row_count],
  struct window_size window_size
);

#endif
