#ifndef __KOHONEN_H__
#define __KOHONEN_H__

#define _BSD_SOURCE

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_draw.h>

#define INPUTS 3
#define EPSILON 0.1
#define STEP 4

typedef struct {
    double weights[INPUTS];
    int x, y;
} Neuron;

typedef struct {
    Neuron *lattice;
    int latice_size;
    double mapRadius;
    int sideX, sideY; 
} Map;

typedef struct {
    double *data;
} Training;


void init_neuron(Neuron *n, int x, int y);
Map* init_map(int xSize, int ySize);

int neuron_distance_to(Neuron *src, Neuron *dst);
double neuron_distance(Neuron *n, double *inputs);

//@name : BMU stands for best matching unit
Neuron* find_bmu(Map *m, double *inputs);

// @args : epsilon is the learning rate
// @args : theta is the influence
void adjust_weights(Neuron *n, double *inputs, double epsilon, double theta);

void epoch(Map *m, double *inputs, int iteration, double timeCst, double *epsilon, int numEpoch);
void train(Map *m, Training *inputs, int num_inputs, int epochs, SDL_Surface *display);

void getColor(Map *m, int x, int y, int *r, int *g, int *b);
    
void destroy_map(Map *m);


#endif
