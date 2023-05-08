#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#include "../common/dependences.h"
#include "clarke_wright_grasp.h"

#define ALPHA 0.5


void copy_routes(int *src, int *dst, int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}


/* GRASP adaptation for the Clarke-Wright savings algorithm */
void clarke_wright_grasp_algorithm(int** distances, Customer *customers, int customers_num, double capacity_max, int max_iterations) {
    int *best_route = (int *) calloc(customers_num, sizeof(int));
    double best_cost = INFINITY;

    for (int iter = 0; iter < max_iterations; iter++) {
        Saving *savings = (Saving *) malloc(customers_num * (customers_num - 1) / 2 * sizeof(Saving));
        bool *visited = (bool *) calloc(customers_num, sizeof(bool));
        int *route = (int *) calloc(customers_num, sizeof(int));
        int saving_index = 0;
        visited[0] = true;

        // Calculate savings and sort them
        for (int i = 0; i < customers_num; i++) {
            for (int j = i + 1; j < customers_num; j++) {
                savings[saving_index].from = i;
                savings[saving_index].to = j;
                savings[saving_index].saving = distances[0][i] +
                                               distances[0][j] -
                                               distances[i][j];
                saving_index++;
            }
        }
        qsort(savings, customers_num * (customers_num - 1) / 2, sizeof(Saving), compare_saving);

        // Select RCL size
        int rcl_size = (int)(ALPHA * customers_num * (customers_num - 1) / 2);

        // GRASP construction phase
        int customers_left = customers_num - 1;
        int route_index = 0;
        double current_load = 0.0;

        while (customers_left > 0) {
            // Choose random savings from the RCL
            int r = rand() % rcl_size;
            int from = savings[r].from;
            int to = savings[r].to;

            if (!visited[from] && !visited[to] &&
                current_load + customers[from].demand + customers[to].demand <= capacity_max) {
                route[route_index++] = from;
                route[route_index++] = to;
                visited[from] = visited[to] = true;
                current_load += customers[from].demand + customers[to].demand;
                customers_left -= 2;
            }
        }

        // Local search phase

        // Update best solution
        double current_cost = 0.0;
        for (int i = 0; i < route_index - 1; i++) {
            current_cost += distances[route[i]][route[i + 1]];
        }
        current_cost += distances[0][route[0]] + distances[0][route[route_index - 1]];

        if (current_cost < best_cost) {
            best_cost = current_cost;
            copy_routes(route, best_route, route_index);
        }

        free(savings);
        free(visited);
        free(route);
    }


    printf("Best route: 0");
    for (int i = 0; i < customers_num - 1; i++) {
        printf(" -> %d", best_route[i]);
    }
    printf(" -> 0\n");
    printf("Cost: %f\n", best_cost);

    free(best_route);
}