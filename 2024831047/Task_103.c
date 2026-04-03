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
    int r, g, b;
} Circle;

Circle circle1;
Circle circle2;

int initialize_window(void){
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        return 0;
    }

    window = SDL_CreateWindow("Task 103",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0);

    if(!window){
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer){
        return 0;
    }

    return 1;
}

void draw_circle(SDL_Renderer* renderer, int cx, int cy, int r) {
    for (int dy = -r; dy <= r; dy++) {
        int dx = (int)sqrt(r * r - dy * dy);
        SDL_RenderDrawLine(renderer, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

void setup(){
   
    circle1.x = 0;
    circle1.y = WINDOW_HEIGHT / 2;
    circle1.radius = 40;
    circle1.r = 0;
    circle1.g = 0;
    circle1.b = 255;

  
    circle2.x = WINDOW_WIDTH / 2;
    circle2.y = 0;
    circle2.radius = 40;
    circle2.r = 0;
    circle2.g = 255;
    circle2.b = 0;
}

void process_input(){
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT)
            gameIsRunning = false;

        if(event.type == SDL_KEYDOWN){
            if(event.key.keysym.sym == SDLK_ESCAPE)
                gameIsRunning = false;

            if(event.key.keysym.sym == SDLK_UP)
                circle2.y -= 10;
            if(event.key.keysym.sym == SDLK_DOWN)
                circle2.y += 10;
            if(event.key.keysym.sym == SDLK_LEFT)
                circle2.x -= 10;
            if(event.key.keysym.sym == SDLK_RIGHT)
                circle2.x += 10;
        }
    }
}

void update(){
    
    circle1.x += 2;

    if(circle1.x > WINDOW_WIDTH){
        circle1.x = 0;
    }

    
    float dx = circle1.x - circle2.x;
    float dy = circle1.y - circle2.y;
    float distance = sqrt(dx*dx + dy*dy);

    if(distance <= (circle1.radius + circle2.radius)){
       
        circle1.r = 255; circle1.g = 0; circle1.b = 0;
        circle2.r = 255; circle2.g = 0; circle2.b = 0;
    } else {
       
        circle1.r = 0; circle1.g = 0; circle1.b = 255;
        circle2.r = 0; circle2.g = 255; circle2.b = 0;
    }
}

void render(){
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_RenderClear(renderer);

   
    SDL_SetRenderDrawColor(renderer, circle1.r, circle1.g, circle1.b, 255);
    draw_circle(renderer, (int)circle1.x, (int)circle1.y, (int)circle1.radius);

    
    SDL_SetRenderDrawColor(renderer, circle2.r, circle2.g, circle2.b, 255);
    draw_circle(renderer, (int)circle2.x, (int)circle2.y, (int)circle2.radius);

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