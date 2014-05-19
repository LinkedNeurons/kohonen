#include <stdlib.h>
#include <stdio.h>
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

    SDL_Surface *display = NULL;
    init(&display);
    Map *m = init_map(10000, 100);
    Training *inputs = malloc(sizeof(Training) * 8);
    
    for(int i = 0; i < 8; ++i) {
        (inputs + i)->data = malloc(3 * sizeof(double));
    }

    inputs[0].data[0] = 0; inputs[0].data[1] = 0;   inputs[0].data[2] = 255;
    inputs[1].data[0] = 0; inputs[1].data[1] = 255; inputs[1].data[2] = 0;
    inputs[2].data[0] = 0; inputs[2].data[1] = 0;   inputs[2].data[2] = 255;

    inputs[3].data[0] = 44;  inputs[3].data[1] = 62;  inputs[3].data[2] = 80;
    inputs[4].data[0] = 192; inputs[4].data[1] = 57;  inputs[4].data[2] = 43;
    inputs[5].data[0] = 41;  inputs[5].data[1] = 128; inputs[5].data[2] = 85;
    inputs[6].data[0] = 211; inputs[6].data[1] = 84;  inputs[6].data[2] = 0;
    inputs[7].data[0] = 142; inputs[7].data[1] = 68;  inputs[7].data[2] = 173;

    train(m, inputs, 8);

    draw(display, m);    

    destroy_map(m);
 
    for(int i = 0; i < 8; ++i) {
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
