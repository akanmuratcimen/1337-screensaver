#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "raylib.h"

struct Cell {
  const char *Value;
  int Column;
  int Row;
  Vector2 Position;
  float PausePosition;
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
const float matchThreshold = 10.0f;
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
      cell.PausePosition = -1.0f;

      cells[i][j] = cell;
    }
  }

  int columnHighlightIsEnabled[columnCount];

  int framesCounter = 0;
  int highlightingFramesCounter = 0;

  while (!WindowShouldClose()) {
    framesCounter++;
    highlightingFramesCounter++;

    if (highlightingFramesCounter > 50) {
      for (int i = 0; i < columnCount - 4; i++) {
        if (columnHighlightIsEnabled[i] == 1) {
          continue;
        }

        for (int j = 0; j < rowCount; j++) {
          if (cells[i + 0][j].Value[0] != '1') {
            continue;
          }

          float maxY = cells[i + 0][j].Position.y;

          int n1Index = -1;
          int n2Index = -1;
          int n3Index = -1;

          for (int n = 0; n < rowCount; n++) {
            if (columnHighlightIsEnabled[i + 1] == 1) {
              continue;
            }

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
              ) > matchThreshold
            ) {
              continue;
            }

            if (cells[i + 1][n].Position.y > maxY) {
              maxY = cells[i + 1][n].Position.y;
            }

            n1Index = n;
          }

          if (n1Index == -1) {
            continue;
          }

          for (int n = 0; n < rowCount; n++) {
            if (columnHighlightIsEnabled[i + 2] == 1) {
              continue;
            }

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
              ) > matchThreshold
            ) {
              continue;
            }

            if (cells[i + 2][n].Position.y > maxY) {
              maxY = cells[i + 2][n].Position.y;
            }

            n2Index = n;
          }

          if (n2Index == -1) {
            continue;
          }

          for (int n = 0; n < rowCount; n++) {
            if (columnHighlightIsEnabled[i + 3] == 1) {
              continue;
            }

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
              ) > matchThreshold
            ) {
              continue;
            }

            if (cells[i + 3][n].Position.y > maxY) {
              maxY = cells[i + 3][n].Position.y;
            }

            n3Index = n;
          }

          if (n3Index == -1) {
            continue;
          }

          cells[i + 0][j].Highlight = 1;
          cells[i + 1][n1Index].Highlight = 1;
          cells[i + 2][n2Index].Highlight = 1;
          cells[i + 3][n3Index].Highlight = 1;

          float column1Diff = maxY - cells[i + 0][j].Position.y;
          float column2Diff = maxY - cells[i + 1][n1Index].Position.y;
          float column3Diff = maxY - cells[i + 2][n2Index].Position.y;
          float column4Diff = maxY - cells[i + 3][n3Index].Position.y;

          for (int m = 0; m < rowCount; m++) {
            cells[i + 0][m].PausePosition =
              cells[i + 0][m].Position.y + column1Diff;
          }

          for (int m = 0; m < rowCount; m++) {
            cells[i + 1][m].PausePosition =
              cells[i + 1][m].Position.y + column2Diff;
          }

          for (int m = 0; m < rowCount; m++) {
            cells[i + 2][m].PausePosition =
              cells[i + 2][m].Position.y + column3Diff;
          }

          for (int m = 0; m < rowCount; m++) {
            cells[i + 3][m].PausePosition =
              cells[i + 3][m].Position.y + column4Diff;
          }

          columnHighlightIsEnabled[i + 0] = 1;
          columnHighlightIsEnabled[i + 1] = 1;
          columnHighlightIsEnabled[i + 2] = 1;
          columnHighlightIsEnabled[i + 3] = 1;
        }
      }
    }

    BeginDrawing();

      ClearBackground(bgColor);

      for (int i = 0; i < columnCount; i++) {
        for (int j = 0; j < rowCount; j++) {
          struct Cell cell = cells[i][j];

          if (
            columnHighlightIsEnabled[i] == 1 &&
            cell.Position.y >= cell.PausePosition
          ) {
            if (cell.Highlight == 1) {
              DrawRectangle(
                cell.Position.x - 8,
                cell.Position.y + 18,
                114,
                146,
                WHITE
              );
            };
          } else {
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
        if (columnHighlightIsEnabled[i] == 1) {
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
