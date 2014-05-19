#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>

#include "kohonen.h"


void init(SDL_Surface **display) {
    SDL_Init(SDL_INIT_VIDEO);
    *display = SDL_SetVideoMode(400, 400, 32, SDL_HWSURFACE);
    SDL_FillRect(*display, NULL, 
        SDL_MapRGB((*display)->format, 44, 62, 80));
    
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



int main() {
    Map *m = init_map(10000, 100);
    double **inputs = malloc(sizeof(double *) * 8);
    
    for(int i = 0; i < 8; ++i) {
        inputs[i] = malloc(3 * sizeof(double));
    }

    inputs[0][1] = 0;
    inputs[0][2] = 0;
    inputs[0][3] = 255;

    inputs[1][1] = 0;
    inputs[1][2] = 255;
    inputs[1][3] = 0;

    inputs[2][1] = 0;
    inputs[2][2] = 0;
    inputs[2][3] = 255;

    inputs[3][1] = 44;
    inputs[3][2] = 62;
    inputs[3][3] = 80;

    inputs[4][1] = 192;
    inputs[4][2] = 57;
    inputs[4][3] = 43;

    inputs[5][1] = 41;
    inputs[5][2] = 128;
    inputs[5][3] = 85;

    inputs[6][1] = 211;
    inputs[6][2] = 84;
    inputs[6][3] = 0;

    inputs[7][1] = 142;
    inputs[7][2] = 68;
    inputs[7][3] = 173;

   train(m, inputs, 8); 

    destroy_map(m);
    return 0;
}
