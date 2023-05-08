#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "dependences.h"
#include "clarke_wright.h"


int compare_saving(const void *a, const void *b) {
    Saving *s1 = (Saving *)a;
    Saving *s2 = (Saving *)b;
    if (s1->saving > s2->saving) return -1;
    if (s1->saving < s2->saving) return 1;
    return 0;
}

void clarke_wright_savings_algorithm(int** distances, Customer *customers, int customers_num, double capacity_max) {
    Saving *savings = (Saving *) malloc(customers_num * (customers_num - 1) / 2 * sizeof(Saving));

    bool *visited = (bool *) calloc(customers_num, sizeof(bool));
    int *route = (int *) calloc(customers_num, sizeof(int));
    int saving_index = 0;

    visited[0] = true;

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

    int customers_left = customers_num - 1;
    int route_index = 0;
    double current_load = 0.0;

    while (customers_left > 0) {
        for (int i = 0; i < n * (n - 1) / 2; i++) {
            int from = savings[i].from;
            int to = savings[i].to;

            if (!visited[from] && !visited[to] &&
                current_load + customers[from].demand + customers[to].demand <= capacity_max) {
                route[route_index++] = from;
                route[route_index++] = to;
                visited[from] = visited[to] = true;
                current_load += customers[from].demand + customers[to].demand;
                customers_left -= 2;
                break;
            }
        }
    }

    printf("Route: 0");
    for (int i = 0; i < customers_num - 1; i++) {
        printf(" -> %d", route[i]);
    }
    printf(" -> 0\n");

    free(savings);
    free(visited);
    free(route);
}