#include "kohonen.h"
#include <stdio.h>

double min_dbl(double a, double b) { return a < b ? a : b; }
double max_dbl(double a, double b) { return a > b ? a : b; }

void init_neuron(Neuron *n, int x, int y) {
    n->x = x; n->y = y;
    for(int i = 0; i < INPUTS; ++i) {
        double r = (double)rand() / (double)RAND_MAX;
        n->weights[i] = r;
    }
}


Map* init_map(int sideX, int sideY) {

    Map *map = malloc(sizeof(Map));
    map->latice_size = sideX * sideY;
    map->mapRadius   = max_dbl(sideX, sideY) / 2;
    map->sideX       = sideX; 
    map->sideY       = sideY; 
    map->lattice     = malloc(map->latice_size * sizeof(Neuron));

    for(int y = 0; y < sideY; ++y) {
        for(int x = 0; x < sideX; ++x) {
            init_neuron(map->lattice + y * sideX + x, x, y);
        }
    }
    return map;
}

double neuron_distance(Neuron *n, double *inputs) {
    double val = 0;
    for(int i = 0; i < INPUTS; ++i) {
        val += (inputs[i] - n->weights[i]) * (inputs[i] - n->weights[i]);
    }
    return val;
}

int neuron_distance_to(Neuron *src, Neuron *dst) {
    return (dst->x - src->x) * (dst->x - src->x)
        +  (dst->y - src->y) * (dst->y - src->y);
}



Neuron* find_bmu(Map *m, double *inputs) {
    Neuron *n = m->lattice;
    double min_val = neuron_distance(n, inputs);

    for(int i = 1; i < m->latice_size; ++i) {
        double curr = neuron_distance(&(m->lattice[i]), inputs);
        min_val = min_dbl(curr, min_val);
        if(min_val == curr) { n = &(m->lattice[i]); }
    }

    return n;
}


void adjust_weights(Neuron *n, double *inputs, double epsilon, double theta) {
    
    for(int i = 0; i < INPUTS; ++i) {
        n->weights[i] += epsilon * theta * (inputs[i] - n->weights[i]);
    }
}


void train(Map *m, Training *inputs, int num_inputs, int numEpoch) {
    int iteration = 0;
    double epsilon = EPSILON;
    double timeCst = numEpoch / log(m->mapRadius); 

    while(iteration < numEpoch) {
        int input_chosen = rand() % num_inputs;
        double *input = (inputs + input_chosen)->data; 
        epoch(m, input, iteration, timeCst, &epsilon, numEpoch);  
        ++iteration;
    }
}



void epoch(Map *m, double *inputs, int iteration, double timeCst, double *epsilon, int numEpoch) {
    double radius = max_dbl(m->mapRadius * exp(-iteration / timeCst), 1); 
    radius *= radius;

    Neuron *n = find_bmu(m, inputs);

    for(int i = 0; i < m->latice_size; ++i) {
        int dst = neuron_distance_to(&(m->lattice[i]), n);
        
        if(dst < radius) {
            double theta = exp(-dst / (2 * radius));
            adjust_weights(&(m->lattice[i]), inputs, *epsilon, theta);

        }
        *epsilon = EPSILON * exp((double)-iteration / (numEpoch- iteration));  
    }
}

void getColor(Map *m, int x, int y, int *r, int *g, int *b) {
    int pos = y * m->sideX + x;
    Neuron *n = &(m->lattice[pos]);

    *r = 255 * n->weights[0];
    *g = 255 * n->weights[1];
    *b = 255 * n->weights[2];

}

void destroy_map(Map *m) {
    free(m->lattice);
    free(m);
}
