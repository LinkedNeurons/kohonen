#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_draw.h>

#include "kohonen.h"

#define WIDTH 400
#define HEIGHT 400
#define STEP 4

void init(SDL_Surface **display) {
    SDL_Init(SDL_INIT_VIDEO);
    *display = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE);
    SDL_FillRect(*display, NULL, 
        SDL_MapRGB((*display)->format, 0, 0, 0));
    
    SDL_Flip(*display);
    SDL_WM_SetCaption("Kohonen Maps", NULL);
}



void catchKillEvent(int *killed) {
    SDL_Event event;
    SDL_WaitEvent(&event);
    switch(event.type) {
        case SDL_QUIT: *killed = 1; break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym) {
                case SDLK_ESCAPE: *killed = 1; break;
                case SDLK_q: *killed = 1;      break;
                default: break;
             }
        break;
        default: return;
    }
}

void setPixel(SDL_Surface *display, int x, int y, int r, int g, int b) {
    int color = r << 16 | g << 8 | b;
    Draw_Pixel(display, x, y, color);
    

}

void draw(SDL_Surface *display, Map *m) {
    int r, g, b;

    for (size_t i = 0; i < WIDTH / STEP; i++)
        for (size_t j = 0; j < HEIGHT / STEP; j++) {
            getColor(m, i, j, &r, &g, &b);
            for (size_t k = 0; k < STEP; k++)
                for (size_t l = 0; l < STEP; l++)
                    setPixel(display, i * STEP  + k, j * STEP + l, r, g, b);                
        }
    SDL_Flip(display);

}

int main() {

    srand(time(NULL));

    SDL_Surface *display = NULL;
    init(&display);
    Map *m = init_map(10000, 100);
    Training *inputs = malloc(sizeof(Training) * 6);
    
    for(int i = 0; i < 6; ++i) {
        (inputs + i)->data = malloc(3 * sizeof(double));
    }
/*
    inputs[0].data[0] = 1; inputs[0].data[1] = 0; inputs[0].data[2] = 0;
    inputs[1].data[0] = 0; inputs[1].data[1] = 1; inputs[1].data[2] = 0;
    inputs[2].data[0] = 0; inputs[2].data[1] = 0; inputs[2].data[2] = 1;

    inputs[3].data[0] = 0.172; inputs[3].data[1] = 0.243; inputs[3].data[2] = 0.313;
    inputs[4].data[0] = 0.752; inputs[4].data[1] = 0.223; inputs[4].data[2] = 0.168;
    inputs[5].data[0] = 0.160; inputs[5].data[1] = 0.501; inputs[5].data[2] = 0.333;
    inputs[6].data[0] = 0.827; inputs[6].data[1] = 0.329; inputs[6].data[2] = 0;
    inputs[7].data[0] = 0.556; inputs[7].data[1] = 0.266; inputs[7].data[2] = 0.678;
*/
    inputs[0].data[0] = 1; inputs[0].data[1] = 0; inputs[0].data[2] = 0;
    inputs[1].data[0] = 0; inputs[1].data[1] = 1; inputs[1].data[2] = 0;
    inputs[2].data[0] = 0; inputs[2].data[1] = 0; inputs[2].data[2] = 1;
    

    inputs[3].data[0] = 0; inputs[3].data[1] = 0.5; inputs[3].data[2] = 0.25;
    inputs[4].data[0] = 0; inputs[4].data[1] = 0;   inputs[4].data[2] = 0.5;
    inputs[5].data[0] = 1; inputs[5].data[1] = 0;   inputs[5].data[2] = 1;
    
    train(m, inputs, 6);

    draw(display, m);    

    destroy_map(m);
 
    for(int i = 0; i < 6; ++i) {
        free((inputs + i)->data);
    }
    free(inputs);

    int killed = 0;
    while(!killed) {
        catchKillEvent(&killed);
    }

    SDL_FreeSurface(display);
    SDL_Quit();

    return 0;
}
