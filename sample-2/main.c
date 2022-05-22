#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"

struct Cell {
  const char *Value;
  int Speed;
  int Column;
  int Row;
  Vector2 Position;
};

const char* fontFamily = "sora.ttf";
const int columnCount = 14;
const char* chars[] = { "1", "3", "7" };
const Color bgColor = { 12, 12, 245, 255 };
const Color fontColor = { 5, 226, 115, 255 };
const int fontSize = 190;
const float lineHeight = 140.0f;

int main(
  void
) {
  int screenWidth = GetScreenWidth();
  int screenHeight = GetScreenHeight();

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);

  InitWindow(screenWidth, screenHeight, "1337 Screensaver");

  const Font font = LoadFontEx(fontFamily, fontSize, 0, 0);

  //ToggleFullscreen();
  SetTargetFPS(60);

  const int columnCount = 40;
  const int rowCount = 32;

  int columnSpeeds[columnCount];

  for (int i = 0; i < columnCount; i++) {
    columnSpeeds[i] = GetRandomValue(10, 20);
  }

  struct Cell ***cells = malloc(sizeof(void *) * columnCount);

  for (int i = 0; i < columnCount; i++) {
    cells[i] = malloc(sizeof(void *) * rowCount);

    for (int j = 0; j < rowCount; j++) {
      const Vector2 position = {
        i * 110 - 40,
        j * lineHeight - 1000
      };

      struct Cell *cell = malloc(sizeof(struct Cell));

      cell->Value = chars[GetRandomValue(0, 2)];
      cell->Speed = columnSpeeds[i];
      cell->Column = i;
      cell->Row = j;
      cell->Position = position;

      cells[i][j] = cell;
    }
  }

  int framesCounter = 0;

  while (!WindowShouldClose()) {
    framesCounter++;

    BeginDrawing();

      ClearBackground(bgColor);

      for (int i = 0; i < columnCount; i++) {
        for (int j = 0; j < rowCount; j++) {
          struct Cell *cell = cells[i][j];

          cell->Position.y += (float) cell->Speed / 10;

          DrawTextEx(
            font,
            cell->Value,
            cell->Position,
            fontSize,
            0,
            fontColor
          );
        }
      }

    EndDrawing();

    if (((framesCounter / 120) % 2) == 1) {
      framesCounter = 0;

      for (int i = 0; i < columnCount; i++) {
        long lastPosition[columnCount];

        for (int j = 0; j < rowCount; j++) {
          if (cells[i][j]->Position.y < lastPosition[i]) {
            lastPosition[i] = cells[i][j]->Position.y;
          }
        }

        for (int j = 0; j < rowCount; j++) {
          if (cells[i][j]->Position.y > GetScreenHeight()) {
            lastPosition[i] = lastPosition[i] - lineHeight;
            cells[i][j]->Position.y = lastPosition[i];
          }
        }
      }
    }
  }

  UnloadFont(font);

  CloseWindow();

  return 0;
}
