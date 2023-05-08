#ifndef CLARKE_WRIGHT_GRASP_H
#define CLARKE_WRIGHT_GRASP_H

#include "../common/clarke_wright.h"

void clarke_wright_grasp_algorithm(int** distances, Customer *customers, int customers_num, double capacity_max, int max_iterations);

#endif // CLARKE_WRIGHT_GRASP_H

