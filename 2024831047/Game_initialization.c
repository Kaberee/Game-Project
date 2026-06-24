#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

/* ─── Window Settings ─────────────────────────────────────────────────────── */
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 640  /* Taller than the grid area for the score banner */
#define BANNER_HEIGHT 40   /* Height of the top score bar */

/* ─── Global State ────────────────────────────────────────────────────────── */
bool          gameIsRunning = false;
SDL_Window   *window        = NULL;
SDL_Renderer *renderer      = NULL;

/* ─── Window Initialisation ───────────────────────────────────────────────── */
int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        return 0; /* SDL failed to start */
    }
    window = SDL_CreateWindow("Snake Engine - Game_initialization",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) return 0;

    /* Accelerated renderer uses the GPU for faster drawing */
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer ? 1 : 0;
}

/* ─── Input Handling ──────────────────────────────────────────────────────── */
void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) gameIsRunning = false; /* Window closed */

        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                gameIsRunning = false; /* Quit the game */
            }
        }
    }
}

/* ─── Rendering ───────────────────────────────────────────────────────────── */
void render(void) {
    /* Clear the screen with a near-black background */
    SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
    SDL_RenderClear(renderer);

    /* Draw the dark score banner bar background across the top */
    SDL_SetRenderDrawColor(renderer, 28, 28, 28, 255);
    SDL_Rect header_rect = { 0, 0, WINDOW_WIDTH, BANNER_HEIGHT };
    SDL_RenderFillRect(renderer, &header_rect);

    /* Push everything we've drawn to the screen */
    SDL_RenderPresent(renderer);
}

/* ─── Cleanup ─────────────────────────────────────────────────────────────── */
void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/* ─── Entry Point ─────────────────────────────────────────────────────────── */
int main(int argc, char *argv[]) {
    gameIsRunning = initialize_window();

    while (gameIsRunning) {
        process_input();
        render();
        SDL_Delay(16); /* Initial loop delay placeholder */
    }

    destroy_window();
    return 0;
}