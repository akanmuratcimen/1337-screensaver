#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "raylib.h"

struct Cell {
  const char *Value;
  int Column;
  int Row;
  Vector2 Position;
  int Highlight;
};

const char* fontFamily = "sora.ttf";
const int columnCount = 40;
const int rowCount = 40;
const char* chars[] = { "1", "3", "7" };
const Color bgColor = { 12, 12, 245, 255 };
const Color fontColor = { 5, 226, 115, 255 };
const int fontSize = 190;
const float lineHeight = 140.0f;

const int speedMin = 2;
const int speedMax = 5;

int main(
  void
) {
  //SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(GetScreenWidth(), GetScreenHeight(), "1337 Screensaver");

  const Font font = LoadFontEx(fontFamily, fontSize, 0, 0);

  ToggleFullscreen();
  SetTargetFPS(30);

  float columnSpeeds[columnCount];

  for (int i = 0; i < columnCount; i++) {
    columnSpeeds[i] = GetRandomValue(speedMin, speedMax) * 1.0f;
  }

  struct Cell cells[rowCount][columnCount];

  for (int i = 0; i < columnCount; i++) {
    for (int j = 0; j < rowCount; j++) {
      const Vector2 position = {
        i * 110.0f - 40.0f,
        j * lineHeight - 1000.0f
      };

      struct Cell cell;

      cell.Value = chars[GetRandomValue(0, 2)];
      cell.Column = i;
      cell.Row = j;
      cell.Position = position;
      cell.Highlight = 0;

      cells[i][j] = cell;
    }
  }

  int framesCounter = 0;
  int highlightingFramesCounter = 0;

  while (!WindowShouldClose()) {
    framesCounter++;
    highlightingFramesCounter++;

    float threshhold = 10.0f;

    if (highlightingFramesCounter > 50) {
      for (int i = 0; i < columnCount - 4; i++) {
        if (columnSpeeds[i] < 0.0f) {
          continue;
        }

        for (int j = 0; j < rowCount; j++) {
          if (cells[i + 0][j].Value[0] != '1') {
            continue;
          }

          int n1Index = -1;
          int n2Index = -1;
          int n3Index = -1;

          for (int n = 0; n < rowCount; n++) {
            if (cells[i + 1][n].Value[0] != '3') {
              continue;
            }

            if (cells[i + 1][n].Position.y < 0) {
              continue;
            }

            if (cells[i + 1][n].Position.y > GetScreenHeight()) {
              continue;
            }

            if (
              abs(
                cells[i + 1][n].Position.y -
                cells[i + 0][j].Position.y
              ) > threshhold
            ) {
              continue;
            }

            n1Index = n;
          }

          if (n1Index == -1) {
            continue;
          }

          for (int n = 0; n < rowCount; n++) {
            if (cells[i + 2][n].Value[0] != '3') {
              continue;
            }

            if (cells[i + 2][n].Position.y < 0) {
              continue;
            }

            if (cells[i + 2][n].Position.y > GetScreenHeight()) {
              continue;
            }

            if (
              abs(
                cells[i + 2][n].Position.y -
                cells[i + 0][j].Position.y
              ) > threshhold
            ) {
              continue;
            }

            n2Index = n;
          }

          if (n2Index == -1) {
            continue;
          }

          for (int n = 0; n < rowCount; n++) {
            if (cells[i + 3][n].Value[0] != '7') {
              continue;
            }

            if (cells[i + 3][n].Position.y < 0) {
              continue;
            }

            if (cells[i + 3][n].Position.y > GetScreenHeight()) {
              continue;
            }

            if (
              abs(
                cells[i + 3][n].Position.y -
                cells[i + 0][j].Position.y
              ) > threshhold
            ) {
              continue;
            }

            n3Index = n;
          }

          if (n3Index == -1) {
            continue;
          }

          columnSpeeds[i + 0] = -1.0f;
          columnSpeeds[i + 1] = -1.0f;
          columnSpeeds[i + 2] = -1.0f;
          columnSpeeds[i + 3] = -1.0f;
        }
      }
    }

    BeginDrawing();

      ClearBackground(bgColor);

      for (int i = 0; i < columnCount; i++) {
        for (int j = 0; j < rowCount; j++) {
          struct Cell cell = cells[i][j];

          if (columnSpeeds[i] > 0) {
            cells[i][j].Position.y += columnSpeeds[i];
          }

          DrawTextEx(
            font,
            cell.Value,
            cell.Position,
            fontSize,
            0,
            fontColor
          );
        }
      }

    EndDrawing();

    if (((framesCounter / 200) % 2) == 1) {
      framesCounter = 0;

      for (int i = 0; i < columnCount; i++) {
        if (columnSpeeds[i] < 0.0f) {
          continue;
        }

        columnSpeeds[i] = GetRandomValue(speedMin, speedMax);

        float lastPosition[columnCount];

        for (int j = 0; j < columnCount; j++) {
          lastPosition[j] = 99999999.0f;
        }

        for (int j = 0; j < rowCount; j++) {
          if (cells[i][j].Position.y < lastPosition[i]) {
            lastPosition[i] = cells[i][j].Position.y;
          }
        }

        for (int j = 0; j < rowCount; j++) {
          if (cells[i][j].Position.y > GetScreenHeight()) {
            lastPosition[i] = lastPosition[i] - lineHeight;
            cells[i][j].Position.y = lastPosition[i];
            cells[i][j].Value = chars[GetRandomValue(0, 2)];
          }
        }
      }
    }
  }

  UnloadFont(font);

  CloseWindow();

  return 0;
}
