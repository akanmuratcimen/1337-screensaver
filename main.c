#include "raylib.h"

int main(
  void
) {
  const int screenWidth = GetScreenWidth();
  const int screenHeight = GetScreenHeight();

  const Color bg_color = { 12, 12, 245, 255 };
  const Color font_color = { 5, 226, 115, 255 };

  InitWindow(screenWidth, screenHeight, "1337 Screensaver");

  ToggleFullscreen();

  const char *text = "133713371337133713371337133713371337133713371337133713371337";
  Font font = LoadFontEx("sora.ttf", 96, 0, 0);

  Image bg_text_image = ImageTextEx(font, text, 72, 0, font_color);
  Texture2D background = LoadTextureFromImage(bg_text_image);

  SetTargetFPS(60);

  const int line_height = 140;

  int scrollingBack = 0;

  while (!WindowShouldClose()) {
    scrollingBack -= 1.0;

    if (scrollingBack <= -background.width * 2) {
      scrollingBack = 0;
    }

    BeginDrawing();

      ClearBackground(bg_color);
      
      for (int i = 0; i < 24; i++) {
        const Vector2 bg1Pos = {
          scrollingBack - 64 * i,
          i * line_height - 24
        };

        const Vector2 bg2Pos = {
          background.width * 2 + scrollingBack - (64 * i),
          i * line_height - 24
        };

        DrawTextureEx(
          background,
          bg1Pos,
          0.0f,
          2.0f,
          WHITE
        );

        DrawTextureEx(
          background,
          bg2Pos,
          0.0f,
          2.0f,
          WHITE
        );
      }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
