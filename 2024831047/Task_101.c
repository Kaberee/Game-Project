#include <SDL2/SDL.h>
#include<stdio.h>
#include <math.h>
#include<stdbool.h>

#define PI 3.14159265
#define screen_width 1080
#define screen_height 520
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

Circle circle;

int initialize_window(void){
    if(SDL_Init(SDL_INIT_EVERYTHING)!=0){
        return 0;
    }
    window = SDL_CreateWindow("Task 101", SDL_WINDOWPOS_CENTERED, 
    SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH,WINDOW_HEIGHT,
    SDL_WINDOW_BORDERLESS);

    if(!window){
        fprintf(stderr,"ERROR creating sdl window.\n");
        return 0;
    }
    renderer = SDL_CreateRenderer(window,-1,0);
    if(!renderer){
        fprintf(stderr, "Error creating SDL renderer.\n");
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
void render(){
  
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

   
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    draw_circle(renderer, (int)circle.x, (int)circle.y, (int)circle.radius);

    
    SDL_RenderPresent(renderer);
}

void setup(){ circle.x = WINDOW_WIDTH / 2;
    circle.y = WINDOW_HEIGHT / 2;
    circle.radius = 100;
    circle.r = 0;
    circle.g = 150;
    circle.b = 255;

}


void process_input(){
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type){
        case SDL_QUIT:
        gameIsRunning = false; break;
        case SDL_KEYDOWN:
    
        if(event.key.keysym.sym == SDLK_ESCAPE)
        gameIsRunning = false; 
        break;

    }
}

void update(){

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
        
    }

    destroy_window();
    return 0;
}