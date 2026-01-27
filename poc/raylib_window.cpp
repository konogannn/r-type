#include <raylib.h>

int main()
{
    InitWindow(800, 600, "Raylib POC");
    SetTargetFPS(60);

    Rectangle player = {400, 300, 50, 50};
    float speed = 300.0f;

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();

        if (IsKeyDown(KEY_RIGHT)) player.x += speed * delta;
        if (IsKeyDown(KEY_LEFT)) player.x -= speed * delta;
        if (IsKeyDown(KEY_DOWN)) player.y += speed * delta;
        if (IsKeyDown(KEY_UP)) player.y -= speed * delta;

        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleRec(player, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
