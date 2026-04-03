#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

bool gameIsRunning = false;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

typedef struct {
    float x, y;
    float radius;
} Circle;

Circle circle;

int initialize_window(void){
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) return 0;

    window = SDL_CreateWindow("Task 102",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if(!window) return 0;

    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer) return 0;

    return 1;
}

void draw_circle(SDL_Renderer* renderer, int cx, int cy, int r) {
    for (int dy = -r; dy <= r; dy++) {
        int dx = (int)sqrt(r * r - dy * dy);
        SDL_RenderDrawLine(renderer, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

void setup(){
    circle.x = WINDOW_WIDTH / 2;
    circle.y = WINDOW_HEIGHT / 2;
    circle.radius = 50;
}

void process_input(){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT) gameIsRunning = false;
        if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            gameIsRunning = false;
    }
}

void update(){
    circle.radius += 0.5;

    if(circle.x + circle.radius >= WINDOW_WIDTH ||
       circle.x - circle.radius <= 0 ||
       circle.y + circle.radius >= WINDOW_HEIGHT ||
       circle.y - circle.radius <= 0){
        circle.radius = 50;
    }
}

void render(){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    draw_circle(renderer, (int)circle.x, (int)circle.y, (int)circle.radius);

    SDL_RenderPresent(renderer);
}

void destroy_window(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]){
    gameIsRunning = initialize_window();
    setup();

    while(gameIsRunning){
        process_input();
        update();
        render();
        SDL_Delay(16);
    }

    destroy_window();
    return 0;
}
