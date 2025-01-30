/*
  GRAVITY BALLS (v1.0.0) - Public Domain - https://github.com/temaxuck/Gravity-Balls

  A very small game written in C using raylib library (https://github.com/raysan5/raylib)

  Game description: Throw colourful balls that bounce off the walls and floor, pretty much like you'd throw Angry Birds.
*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h"

#define DEBUG false

// PHYSICS 
#define MAX_FPS 120
#define MAX_BALL_SPEED 10 
#define GRAVITY 10.0f
#define FRICTION_X 5 // force that reduces balls' horizontal speed when they hit the walls (in %)
#define FRICTION_Y 0 // force that reduces balls' vertical speed when they hit the floor (in %) 

// APPEARANCE
#define BACKGROUND_COLOR GetColor(0x181818FF)
#define DEFAULT_FONT_SIZE 24

typedef struct {
  unsigned int width; 
  unsigned int height;
  const char *title; 
} Window;

typedef struct {
  float radius;
  Color color;
  Vector2 pos;
  Vector2 velocity;
} Ball;

typedef struct {
  Ball *balls;
  size_t ballsCount;
  Vector2 ballSpawnPos;
  Window *window;
} GameState;

void ShowDebugInfo() {
  char infoLabel[100];
  Vector2 labelPos = {.x = 0, .y = 0};

  sprintf(infoLabel, "FPS: %d\nTime elapsed: %.1f\n", GetFPS(), GetTime());
  DrawText(infoLabel, labelPos.x, labelPos.y, DEFAULT_FONT_SIZE, GREEN);
}

Color GetRandomColor() {
  return ColorFromHSV((float)rand() / RAND_MAX * 360.0f, 1.0f, 1.0f);
}

void DrawBall(Ball *b) {
  DrawCircle(b->pos.x, b->pos.y, b->radius, b->color);
}

void UpdateBall(Ball *b, Window *w) {
  if (b->pos.x - b->radius < 0) {
    b->pos.x = b->radius;
  } else if (b->pos.x + b->radius > w->width) {
    b->pos.x = w->width - b->radius;
  }

  if (b->pos.y + b->radius > w->height) {
    b->pos.y = w->height - b->radius;
  }
  
  float dt = GetFrameTime();
  Vector2 nextPos = Vector2Add(b->pos, b->velocity);

  if (nextPos.x + b->radius > w->width || nextPos.x - b->radius < 0) {
    b->velocity.x *= FRICTION_X * 0.01f - 1.0f;
  }

  if (nextPos.y + b->radius > w->height) {
    b->velocity.y *= FRICTION_Y * 0.01f - 1.0f;
    if (fabs(b->velocity.y) > 1.0f) {
      b->color = GetRandomColor();
    }
  }
  
  b->velocity.y += GRAVITY * dt;

  #if DEBUG
    char label[40]; 
    sprintf(label, "Pos: (%.2f, %.2f);\nVel: (%.2f, %.2f)", b->pos.x, b->pos.y, b->velocity.x, b->velocity.y);
    DrawText(label, b->pos.x, b->pos.y, DEFAULT_FONT_SIZE * 0.8f, WHITE);  
  #endif
  b->pos = nextPos;
}

void SpawnBall(GameState *s, Vector2 pos, Vector2 vel, float magnitude) {
  Ball *newBalls = realloc(s->balls, sizeof(Ball) * (s->ballsCount + 1));
  assert(newBalls != NULL); // Couldn't allocate more space for an array on the heap
  s->balls = newBalls;

  Ball spawnedBall = (Ball){
    .radius = fmin(s->window->width, s->window->height) * 0.168 * 0.1,
    .color = GetRandomColor(),
    .pos = pos,
    .velocity = Vector2Scale(Vector2Normalize(vel), magnitude),
  };

  s->balls[s->ballsCount] = spawnedBall;
  s->ballsCount += 1;
}


void Update(GameState *s) {
  s->window->width = GetScreenWidth();
  s->window->height = GetScreenHeight();
  ClearBackground(BACKGROUND_COLOR);
  #if DEBUG
    ShowDebugInfo();
  #endif

  for (size_t i = 0; i < s->ballsCount; i++) {
    DrawBall(&s->balls[i]);
    UpdateBall(&s->balls[i], s->window);
  }

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    s->ballSpawnPos = GetMousePosition();
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    Vector2 vel = Vector2Subtract(s->ballSpawnPos, GetMousePosition());
    SpawnBall(s, s->ballSpawnPos, vel, fmin(MAX_BALL_SPEED, Vector2Length(vel)));
  }
}

int main(void) {
  srand(time(NULL));

  Window w = {
    .width = 1280,
    .height = 720,
    .title = "Gravity Balls",
  };

  GameState s = {
    .window = &w,
    .balls = NULL,
  };

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(w.width, w.height, w.title);
  SetTargetFPS(MAX_FPS);
  
  while (!WindowShouldClose()) {
    BeginDrawing();
    Update(&s);
    EndDrawing();
  }

  free(s.balls);

  return 0;
}
