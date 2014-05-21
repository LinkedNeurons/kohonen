#include "kohonen.h"
#include <stdio.h>

double min_dbl(double a, double b) { return a < b ? a : b; }
double max_dbl(double a, double b) { return a > b ? a : b; }


void setPixel(SDL_Surface *display, int x, int y, int r, int g, int b) {
    int color = r << 16 | g << 8 | b;
    Draw_Pixel(display, x, y, color);
}

int r, g, b;
void draw(SDL_Surface *display, Map *m) {

    for (size_t i = 0; i < (size_t)m->sideX; i++)
    for (size_t j = 0; j < (size_t)m->sideY; j++) {
        getColor(m, i, j, &r, &g, &b);
        for (size_t k = 0; k < (size_t)m->scale; k++)
        for (size_t l = 0; l < (size_t)m->scale; l++)
        setPixel(display, i * m->scale  + k, j * m->scale + l, r, g, b);
    }
    SDL_Flip(display);
}



void init_neuron(Neuron *n, int x, int y, int num_weights) {
    n->x = x; n->y = y;
    n->num_weights = num_weights;
    n->weights = malloc(num_weights * sizeof(double));
    for(int i = 0; i < num_weights; ++i) {
        double r = (double)rand() / (double)RAND_MAX;
        n->weights[i] = r;
    }
}


Map* init_map(int sideX, int sideY, int num_weights, int scale) {

    Map *map = malloc(sizeof(Map));
    map->latice_size = sideX * sideY;
    map->mapRadius   = max_dbl(sideX, sideY) / 2;
    map->sideX       = sideX; 
    map->sideY       = sideY; 
    map->scale       = scale;
    map->lattice     = malloc(map->latice_size * sizeof(Neuron));

    for(int y = 0; y < sideY; ++y) {
        for(int x = 0; x < sideX; ++x) {
            init_neuron(map->lattice + y * sideX + x, x, y, num_weights);
        }
    }
    return map;
}

double neuron_distance(Neuron *n, double *inputs) {
    double val = 0;
    for(int i = 0; i < n->num_weights; ++i) {
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
    
    for(int i = 0; i < n->num_weights; ++i) {
        n->weights[i] += epsilon * theta * (inputs[i] - n->weights[i]);
    }
}


void train(Map *m, Training *inputs, int num_inputs, int numEpoch, SDL_Surface *display) {
    int iteration = 0;
    double epsilon = EPSILON;
    double timeCst = numEpoch / log(m->mapRadius); 

    while(iteration < numEpoch) {
        int input_chosen = rand() % num_inputs;
        double *input = (inputs + input_chosen)->data; 
        epoch(m, input, iteration, timeCst, &epsilon, numEpoch);  
        ++iteration;
        draw(display, m);
        if(iteration % 100 == 0) {
            printf("Epoch %d / %d\n", iteration, numEpoch);
        }
    }
}


double distance_between_weights(Neuron *src, Neuron *dst) {
    double dist = 0;
    for(int i = 0; i < src->num_weights; ++i) {
        dist += dst->weights[i] - src->weights[i];
    }
    return dist < 0 ? -dist : dist;
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

int get_node_quality(Map *m, int x, int y, Neuron *n) {
    double quality = 0.;

    if(x != 0) { // left Neuron (if exists)  
        quality += neuron_distance_to(n, &(m->lattice[y * m->sideX + x - 1])); 
    }
    if(x != m->sideX - 1) { //right Neuron
        quality += neuron_distance_to(n, &(m->lattice[y * m->sideX + x + 1])); 

    }
    if(y != 0) { // top neuron  
        quality += neuron_distance_to(n, &(m->lattice[(y - 1) * m->sideX + x])); 
    }
    if(y != m->sideY - 1) { // bottom Neuron
        quality += neuron_distance_to(n, &(m->lattice[(y + 1) * m->sideX + x])); 
    }

    return quality;
}

void make_quality_map(Map *m, SDL_Surface *display) {
    for(int y = 0; y < m->sideY; ++y)
    for(int x = 0; x < m->sideX; ++x) { 
        int quality = get_node_quality(m, x, y, m->lattice + y * m->sideX +x);
		int grayscale = 255 / (1 + quality);
        for (size_t k = 0; k < (size_t)m->scale; k++)
        for (size_t l = 0; l < (size_t)m->scale; l++)
            setPixel(display, x * m->scale  + k, y * m->scale + l, grayscale, grayscale, grayscale);
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
    for(int i = 0; i < m->latice_size; ++i) {
        free(m->lattice[i].weights);
    }
    free(m->lattice);
    free(m);
}
