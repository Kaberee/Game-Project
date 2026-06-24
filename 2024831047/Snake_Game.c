#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 640  /* Increased height to accommodate the top score banner */
#define BANNER_HEIGHT 40
#define CELL_SIZE     20
#define GRID_COLS     (WINDOW_WIDTH  / CELL_SIZE)
#define GRID_ROWS     ((WINDOW_HEIGHT - BANNER_HEIGHT) / CELL_SIZE)
#define GAME_SPEED_MS 100
#define MAX_SNAKE_LEN (GRID_COLS * GRID_ROWS)

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

typedef struct {
    int x, y;
} Segment;

typedef struct {
    Segment   body[MAX_SNAKE_LEN];
    int       length;
    Direction direction;
    Direction next_direction;
} Snake;

typedef struct {
    int x, y;
} Food;

/* ── Globals ────────────────────────────────────────────────────────────── */
bool          gameIsRunning = false;
bool          gameOver      = false;
SDL_Window   *window        = NULL;
SDL_Renderer *renderer      = NULL;

Snake snake;
Food  food;
int   score = 0;

/* Custom 3x5 bit-matrix font to cleanly render numbers 0-9 */
const unsigned char font_matrix[10][5] = {
    {0xF,0x9,0x9,0x9,0xF}, // 0
    {0x4,0xC,0x4,0x4,0xE}, // 1
    {0xF,0x1,0xF,0x8,0xF}, // 2
    {0xF,0x1,0xF,0x1,0xF}, // 3
    {0x9,0x9,0xF,0x1,0x1}, // 4
    {0xF,0x8,0xF,0x1,0xF}, // 5
    {0xF,0x8,0xF,0x9,0xF}, // 6
    {0xF,0x1,0x2,0x4,0x4}, // 7
    {0xF,0x9,0xF,0x9,0xF}, // 8
    {0xF,0x9,0xF,0x1,0xF}  // 9
};

/* Hardcoded custom pixel display tracking for "GAME OVER" typography without fonts */
const unsigned char game_over_logo[8][5] = {
    {0xF,0x8,0xB,0x9,0xF}, // G
    {0xF,0x9,0xF,0x9,0x9}, // A
    {0x9,0xF,0x9,0x9,0x9}, // M
    {0xF,0x8,0xF,0x8,0xF}, // E
    {0xF,0x9,0x9,0x9,0xF}, // O
    {0x9,0x9,0x9,0x9,0xF}, // V
    {0xF,0x8,0xF,0x8,0xF}, // E
    {0xF,0x9,0xF,0xA,0x9}  // R
};

/* ── Forward Declarations ───────────────────────────────────────────────── */
int  initialize_window(void);
void setup(void);
void process_input(void);
void update(void);
void render(void);
void destroy_window(void);
void spawn_food(void);
void draw_circle(int centerX, int centerY, int radius, int r, int g, int b);
void draw_ui_number(const char* text, int x, int y, int scale);
void draw_game_over_word(int x, int y, int scale);

int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        return 0;
    }
    window = SDL_CreateWindow("Snake Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) return 0;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer ? 1 : 0;
}

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
    score    = 0;
    gameOver = false;
    spawn_food();
}

void spawn_food(void) {
    bool valid;
    do {
        valid = true;
        food.x = rand() % GRID_COLS;
        food.y = rand() % GRID_ROWS;
        for (int i = 0; i < snake.length; i++) {
            if (snake.body[i].x == food.x && snake.body[i].y == food.y) {
                valid = false;
                break;
            }
        }
    } while (!valid);
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
                case SDLK_r:     if (gameOver) setup(); break;
            }
        }
    }
}

void update(void) {
    if (gameOver) return;

    snake.direction = snake.next_direction;
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

    if (snake.body[0].x < 0 || snake.body[0].x >= GRID_COLS ||
        snake.body[0].y < 0 || snake.body[0].y >= GRID_ROWS) {
        gameOver = true;
        return;
    }

    for (int i = 1; i < snake.length; i++) {
        if (snake.body[i].x == snake.body[0].x && snake.body[i].y == snake.body[0].y) {
            gameOver = true;
            return;
        }
    }

    if (snake.body[0].x == food.x && snake.body[0].y == food.y) {
        if (snake.length < MAX_SNAKE_LEN) {
            snake.body[snake.length] = tail_tip;
            snake.length++;
        }
        score++;
        spawn_food();
    }
}

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

void draw_ui_number(const char* text, int x, int y, int scale) {
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    while (*text) {
        int idx = -1;
        if (*text >= '0' && *text <= '9') idx = *text - '0';

        if (idx != -1) {
            for (int row = 0; row < 5; row++) {
                for (int col = 0; col < 4; col++) {
                    if ((font_matrix[idx][row] >> (3 - col)) & 1) {
                        SDL_Rect pixel = { x + col * scale, y + row * scale, scale, scale };
                        SDL_RenderFillRect(renderer, &pixel);
                    }
                }
            }
        }
        x += 5 * scale;
        text++;
    }
}

void draw_game_over_word(int x, int y, int scale) {
    SDL_SetRenderDrawColor(renderer, 240, 70, 70, 255); // Crimson text color
    for (int letter = 0; letter < 8; letter++) {
        if (letter == 4) x += 4 * scale; // Add spacing between words 'GAME' and 'OVER'
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 4; col++) {
                if ((game_over_logo[letter][row] >> (3 - col)) & 1) {
                    SDL_Rect pixel = { x + col * scale, y + row * scale, scale, scale };
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
        x += 5 * scale;
    }
}

void render(void) {
    // Clear whole buffer
    SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
    SDL_RenderClear(renderer);

    // Draw Score Header Banner Background
    SDL_SetRenderDrawColor(renderer, 28, 28, 28, 255);
    SDL_Rect header_rect = { 0, 0, WINDOW_WIDTH, BANNER_HEIGHT };
    SDL_RenderFillRect(renderer, &header_rect);

    // Render Live Score Text inside the banner
    char scoreStr[16];
    snprintf(scoreStr, sizeof(scoreStr), "%d", score);
    draw_ui_number(scoreStr, WINDOW_WIDTH / 2 - 10, 10, 4);

    // Render Food (Smooth Non-Pixelated Red Circle)
    int foodX = food.x * CELL_SIZE + CELL_SIZE / 2;
    int foodY = food.y * CELL_SIZE + CELL_SIZE / 2 + BANNER_HEIGHT;
    draw_circle(foodX, foodY, CELL_SIZE / 2 - 1, 235, 70, 70);

    // Render Snake (Smooth Non-Pixelated Green Circles)
    for (int i = 0; i < snake.length; i++) {
        int cx = snake.body[i].x * CELL_SIZE + CELL_SIZE / 2;
        int cy = snake.body[i].y * CELL_SIZE + CELL_SIZE / 2 + BANNER_HEIGHT;
        if (i == 0) {
            draw_circle(cx, cy, CELL_SIZE / 2, 90, 245, 90);
        } else {
            draw_circle(cx, cy, CELL_SIZE / 2 - 1, 50, 180, 50);
        }
    }

    // GAME OVER PAGE OVERLAY STATE
    if (gameOver) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer,  5,  5,  5, 225); // Opaque backdrop panel
        SDL_Rect overlay = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        SDL_RenderFillRect(renderer, &overlay);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        // Display Custom Rendered Word Art "GAME OVER"
        int wordArtWidth = (9 * 5 * 8);
        draw_game_over_word((WINDOW_WIDTH / 2) - (wordArtWidth / 2), (WINDOW_HEIGHT / 2) - 80, 8);

        // Display Large Final Score directly beneath the Title
        int digitCount = strlen(scoreStr);
        int textWidthOffset = (digitCount * 5 * 10) / 2;
        draw_ui_number(scoreStr, (WINDOW_WIDTH / 2) - textWidthOffset, (WINDOW_HEIGHT / 2) - 10, 10);
    }

    SDL_RenderPresent(renderer);
}

void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
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

    destroy_window();
    return 0;
}
