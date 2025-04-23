/*
  GRAVITY BALLS (v1.0.1) - Public Domain - https://github.com/temaxuck/Gravity-Balls

  A very small game written in C using raylib library (https://github.com/raysan5/raylib)

  Game description: Throw those colourful bouncy juicy balls, pretty much like
                    you'd throw Angry Birds.
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
#define MARKER_SIZE 10
#define MARKER_COLOR GetColor(0xFFFFFFFF)
#define DOTTED_LINE_DOT_SIZE 3

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

  bool isSpawningBall;
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

void DrawDottedLine(Vector2 startPos, Vector2 endPos, float dotSize, Color color) {
  assert(dotSize > 0.0f && "Dot size must be greater than 0");
    
  float progress = .0f;
  float step = 2 * dotSize / Vector2Distance(startPos, endPos);

  while (progress < 1.0f) {
    DrawCircleV(Vector2Lerp(startPos, endPos, progress), dotSize, color);
    progress += 2 * step;
  }
}

void DrawBall(Ball *b) {
  DrawCircle(b->pos.x, b->pos.y, b->radius, b->color);
}

void HandleCollision(Ball *b1, Ball *b2) {
  // J = J1 = -J2 (J - impulse, the conservation of the momentum law)
  // J = lambda * n
  // V1 = v1 + lambda / m1 * n
  // V2 = v2 - lambda / m2 * n
  // Since we ignore mass, or say it equals to 1:
  // V1 = v1 + lambda * n
  // V2 = v2 - lambda * n

  if (CheckCollisionCircles(b1->pos, b1->radius, b2->pos, b2->radius)) {
    Vector2 dx = Vector2Subtract(b2->pos, b1->pos); // position difference
    float dxLength = Vector2Length(dx); // distance between balls' center

    // Normal vector of the forces applied: dx / dxLength
    Vector2 n = Vector2Scale(dx, 1.0f / dxLength);
    // Relative velocity of the first ball towards the second: v1 - v2
    Vector2 dV = Vector2Subtract(b1->velocity, b2->velocity);

    float n_dV = Vector2DotProduct(n, dV); // <n, dV>
    // lambda: -2 * m1*m2 / (m1 + m2) * <n, dV>
    // Since we suppose that all the masses are equal to 1 the lambda factor
    // equals to the negative value of the dot product of the normal and
    // relative velocity.
    float lambda = -n_dV;
    Vector2 factor = Vector2Scale(n, lambda); // lambda * n

    b1->velocity = Vector2Add(b1->velocity, factor);
    b2->velocity = Vector2Subtract(b2->velocity, factor);
  }
}

void UpdateBall(Ball *b, Window *w) {
  // TODO: Move walls and floor collision to another function. 
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
  // TODO: Instead of doing realloc every time we spawn a ball, fill array until
  // the items count has reached capacity and then incrementally extend it's
  // capacity
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
    for (size_t j = i + 1; j < s->ballsCount; j++) {
      HandleCollision(&s->balls[i], &s->balls[j]);
    }
    DrawBall(&s->balls[i]);
    UpdateBall(&s->balls[i], s->window);
  }

  if (s->isSpawningBall) {
    DrawCircle(s->ballSpawnPos.x, s->ballSpawnPos.y, MARKER_SIZE, MARKER_COLOR);
    DrawDottedLine(GetMousePosition(), s->ballSpawnPos, DOTTED_LINE_DOT_SIZE, MARKER_COLOR);
  }

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    s->ballSpawnPos = GetMousePosition();
    s->isSpawningBall = 1;
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    Vector2 vel = Vector2Subtract(s->ballSpawnPos, GetMousePosition());
    SpawnBall(s, s->ballSpawnPos, vel, fmin(MAX_BALL_SPEED, Vector2Length(vel)));
    s->isSpawningBall = 0;
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
