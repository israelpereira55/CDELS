#ifndef CLARKE_WRIGHT_H
#define CLARKE_WRIGHT_H

#include "dependences.h"

typedef struct saving Saving;

struct saving {
    int from, to;
    double saving;
};


int compare_saving(const void *a, const void *b);

void clarke_wright_savings_algorithm(int** distances, Customer *customers, int customers_num, double capacity_max);


#endif // CLARKE_WRIGHT_H
