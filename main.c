#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_draw.h>
#include <string.h>

#include "kohonen.h"

#define WIDTH  400
#define HEIGHT 400

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


void make_random_training_set(Training *inputs, int num) {
    for(int i = 0; i < num; ++i) {
        for(int j = 0; j < 3; ++j) {
            inputs[i].data[j] = (double)rand() / RAND_MAX;
        }
    }
}

void make_default_training_set(Training *inputs) {
    inputs[0].data[0] = 1; inputs[0].data[1] = 0; inputs[0].data[2] = 0;
    inputs[1].data[0] = 0; inputs[1].data[1] = 1; inputs[1].data[2] = 0;
    inputs[2].data[0] = 0; inputs[2].data[1] = 0; inputs[2].data[2] = 1;

    inputs[3].data[0] = 0.172; inputs[3].data[1] = 0.243; inputs[3].data[2] = 0.313;
    inputs[4].data[0] = 0.752; inputs[4].data[1] = 0.223; inputs[4].data[2] = 0.168;
    inputs[5].data[0] = 0.160; inputs[5].data[1] = 0.501; inputs[5].data[2] = 0.333;
    inputs[6].data[0] = 0.827; inputs[6].data[1] = 0.329; inputs[6].data[2] = 0;
    inputs[7].data[0] = 0.556; inputs[7].data[1] = 0.266; inputs[7].data[2] = 0.678; 
}

void print_usage() {
    printf("usage 1 : kohonen isTrainingSetRandom NetSizeX NetSizeY epochs\n");
    printf("usage 2 : kohonen --defaultParams\n");
}

int main(int argc, char **argv) {
    srand(time(NULL));
    
    SDL_Surface *display = NULL;
    int isTrainingSetRandom = 0;
    int netSizeX = 100, netSizeY = 100;
    int trainingSet = 8;
    int epochs = 3000;

    if(argc == 1) {
        print_usage();
        return 0;
    } else if(argc == 2) {
        if(strcmp(argv[1], "--defaultParams")) {
            print_usage();
            return 0;
        }
    } else if(argc == 5) {
        isTrainingSetRandom = atoi(argv[1]);
        netSizeX = atoi(argv[2]);
        netSizeY = atoi(argv[3]);
        epochs   = atoi(argv[4]);
    } else { 
        print_usage();
        return 0;
    }
    init(&display);

    if(isTrainingSetRandom) {
        trainingSet = rand() % 100;
    }

    Training *inputs = malloc(sizeof(Training) * trainingSet);
    for(int i = 0; i < trainingSet; ++i) {
        (inputs + i)->data = malloc(3 * sizeof(double));
    }

    if(isTrainingSetRandom) {
        make_random_training_set(inputs, trainingSet);
    } else {
        make_default_training_set(inputs);
    }

    Map *m = init_map(netSizeX, netSizeY);

    train(m, inputs, trainingSet, epochs, display);

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
