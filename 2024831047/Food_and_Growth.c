#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

/* ─── Window & Grid Settings ─────────────────────────────────────────────── */
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 640
#define BANNER_HEIGHT 40
#define CELL_SIZE     20
#define GRID_COLS     (WINDOW_WIDTH  / CELL_SIZE)
#define GRID_ROWS     ((WINDOW_HEIGHT - BANNER_HEIGHT) / CELL_SIZE)
#define GAME_SPEED_MS 100
#define MAX_SNAKE_LEN (GRID_COLS * GRID_ROWS)

/* ─── Types ───────────────────────────────────────────────────────────────── */
typedef enum { UP, DOWN, LEFT, RIGHT } Direction;
typedef struct { int x, y; } Segment;
typedef struct {
    Segment   body[MAX_SNAKE_LEN];
    int       length;
    Direction direction;
    Direction next_direction; 
} Snake;
typedef struct { int x, y; } Food;

/* ─── Global State ────────────────────────────────────────────────────────── */
bool          gameIsRunning = false;
SDL_Window   *window        = NULL;
SDL_Renderer *renderer      = NULL;
Snake snake;
Food  food;
int   score = 0;

/* ─── Window Initialisation ───────────────────────────────────────────────── */
int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return 0;
    window = SDL_CreateWindow("Snake Engine - Food_and_Growth",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) return 0;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer ? 1 : 0;
}

/* ─── Food Spawning ───────────────────────────────────────────────────────── */
void spawn_food(void) {
    bool valid;
    do {
        valid = true;
        food.x = rand() % GRID_COLS;
        food.y = rand() % GRID_ROWS;

        /* Reject position if any snake segment occupies it */
        for (int i = 0; i < snake.length; i++) {
            if (snake.body[i].x == food.x && snake.body[i].y == food.y) {
                valid = false;
                break;
            }
        }
    } while (!valid);
}

/* ─── Game Setup ──────────────────────────────────────────────────────────── */
void setup(void) {
    srand((unsigned int)time(NULL));
    snake.length         = 3;
    snake.direction      = RIGHT;
    snake.next_direction = RIGHT;

    int start_col = GRID_COLS / 2;
    int start_row = GRID_ROWS / 2;
    for (int i = 0; i < snake.length; i++) {
        snake.body[i].x = start_col - i;
        snake.body[i].y = start_row;
    }
    score = 0;
    spawn_food();
}

/* ─── Drawing Helpers ─────────────────────────────────────────────────────── */
void draw_circle(int centerX, int centerY, int radius, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

/* ─── Input Handling ──────────────────────────────────────────────────────── */
void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) gameIsRunning = false;
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE: gameIsRunning = false; break;
                case SDLK_UP:    if (snake.direction != DOWN)  snake.next_direction = UP;    break;
                case SDLK_DOWN:  if (snake.direction != UP)    snake.next_direction = DOWN;  break;
                case SDLK_LEFT:  if (snake.direction != RIGHT) snake.next_direction = LEFT;  break;
                case SDLK_RIGHT: if (snake.direction != LEFT)  snake.next_direction = RIGHT; break;
            }
        }
    }
}

/* ─── Game Logic Update ───────────────────────────────────────────────────── */
void update(void) {
    snake.direction = snake.next_direction;

    /* Cache tail tip before shifting body for potential growth */
    Segment tail_tip = snake.body[snake.length - 1];

    for (int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }

    switch (snake.direction) {
        case UP:    snake.body[0].y--; break;
        case DOWN:  snake.body[0].y++; break;
        case LEFT:  snake.body[0].x--; break;
        case RIGHT: snake.body[0].x++; break;
    }

    /* Check head to food collisions */
    if (snake.body[0].x == food.x && snake.body[0].y == food.y) {
        if (snake.length < MAX_SNAKE_LEN) {
            snake.body[snake.length] = tail_tip;
            snake.length++;
        }
        score++;
        spawn_food();
    }
}

/* ─── Rendering ───────────────────────────────────────────────────────────── */
void render(void) {
    SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 28, 28, 28, 255);
    SDL_Rect header_rect = { 0, 0, WINDOW_WIDTH, BANNER_HEIGHT };
    SDL_RenderFillRect(renderer, &header_rect);

    /* Draw food as red circle */
    int foodX = food.x * CELL_SIZE + CELL_SIZE / 2;
    int foodY = food.y * CELL_SIZE + CELL_SIZE / 2 + BANNER_HEIGHT;
    draw_circle(foodX, foodY, CELL_SIZE / 2 - 1, 235, 70, 70);

    /* Draw Snake */
    for (int i = 0; i < snake.length; i++) {
        int cx = snake.body[i].x * CELL_SIZE + CELL_SIZE / 2;
        int cy = snake.body[i].y * CELL_SIZE + CELL_SIZE / 2 + BANNER_HEIGHT;
        if (i == 0) {
            draw_circle(cx, cy, CELL_SIZE / 2,     90, 245,  90);
        } else {
            draw_circle(cx, cy, CELL_SIZE / 2 - 1, 50, 180,  50);
        }
    }

    SDL_RenderPresent(renderer);
}

/* ─── Entry Point ─────────────────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    gameIsRunning = initialize_window();
    setup();

    Uint32 last_tick = SDL_GetTicks();
    while (gameIsRunning) {
        process_input();
        Uint32 now = SDL_GetTicks();
        if (now - last_tick >= GAME_SPEED_MS) {
            update();
            last_tick = now;
        }
        render();
        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}