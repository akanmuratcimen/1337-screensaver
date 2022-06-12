#ifndef TEXTURE_H
#define TEXTURE_H

#include "core.h"

void
draw_bulk(
  int column_count,
  int row_count,
  struct cell cells[column_count][row_count]
);

#endif
