#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 640
#define BANNER_HEIGHT 40
#define CELL_SIZE     20
#define GRID_COLS     (WINDOW_WIDTH  / CELL_SIZE)
#define GRID_ROWS     ((WINDOW_HEIGHT - BANNER_HEIGHT) / CELL_SIZE)
#define GAME_SPEED_MS 100 
#define MAX_SNAKE_LEN (GRID_COLS * GRID_ROWS)

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;
typedef struct { int x, y; } Segment;
typedef struct {
    Segment   body[MAX_SNAKE_LEN];
    int       length;
    Direction direction;
    Direction next_direction;
} Snake;

bool gameIsRunning = false;
SDL_Window   *window    = NULL;
SDL_Renderer *renderer  = NULL;
Snake snake;

int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return 0;
    window = SDL_CreateWindow("Snake Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) return 0;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer ? 1 : 0;
}

void setup(void) {
    snake.length = 3;
    snake.direction = RIGHT;
    snake.next_direction = RIGHT;
    int start_col = GRID_COLS / 2;
    int start_row = GRID_ROWS / 2;
    for (int i = 0; i < snake.length; i++) {
        snake.body[i].x = start_col - i;
        snake.body[i].y = start_row;
    }
}

void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) gameIsRunning = false;
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE: gameIsRunning = false; break;
                case SDLK_UP:    if (snake.direction != DOWN) snake.next_direction = UP; break;
                case SDLK_DOWN:  if (snake.direction != UP)   snake.next_direction = DOWN; break;
                case SDLK_LEFT:  if (snake.direction != RIGHT) snake.next_direction = LEFT; break;
                case SDLK_RIGHT: if (snake.direction != LEFT)  snake.next_direction = RIGHT; break;
                case SDLK_w:     if (snake.direction != DOWN) snake.next_direction = UP; break;
                case SDLK_s:     if (snake.direction != UP)   snake.next_direction = DOWN; break;
                case SDLK_a:     if (snake.direction != RIGHT) snake.next_direction = LEFT; break;
                case SDLK_d:     if (snake.direction != LEFT)  snake.next_direction = RIGHT; break;
            }
        }
    }
}

void update(void) {
    snake.direction = snake.next_direction;
    for (int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }
    switch (snake.direction) {
        case UP:    snake.body[0].y--; break;
        case DOWN:  snake.body[0].y++; break;
        case LEFT:  snake.body[0].x--; break;
        case RIGHT: snake.body[0].x++; break;
    }
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
    SDL_RenderClear(renderer);
    
    // Simple block rendering for the snake to confirm it works
    SDL_SetRenderDrawColor(renderer, 90, 245, 90, 255);
    for (int i = 0; i < snake.length; i++) {
        SDL_Rect rect = { snake.body[i].x * CELL_SIZE, snake.body[i].y * CELL_SIZE + BANNER_HEIGHT, CELL_SIZE - 1, CELL_SIZE - 1 };
        SDL_RenderFillRect(renderer, &rect);
    }
    SDL_RenderPresent(renderer);
}

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