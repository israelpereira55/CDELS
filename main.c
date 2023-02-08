#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include "common/local_search.h"
#include "common/dependences.h"
#include "common/io_tools.h"
#include "metaheuristic/differential_evolution.h"

int NP;

int main() {
    int seed = time(NULL);
    srand(seed);
    
    int customers_num = 0,
        vehicles_num = 0,
        best_solution = 0,
        capacity_max = 0;

    enum DETechnique de_technique = terminal_choose_de_technique();
        
    char filename[81];

    printf("Instance file: ");
    if (scanf("%s", filename) != 1) { 
        printf("[ERROR]: IO error.\n"); 
        exit(1); 
    }

    FILE* file_read = fopen(filename, "r");
    if (file_read == NULL) {
        printf("File %s was not found.\nAborting...\n", filename);
        exit(1);
    }
    
    if (fscanf(file_read, "%d %d %d %d", &vehicles_num, &best_solution, &customers_num, &capacity_max) != 4) {
        printf("[ERROR]: IO error.\n"); 
        exit(1); 
    }
    
    Customer customers[customers_num];
    file_customers_init(customers, file_read, customers_num);
    fclose(file_read);

    NP = 3 * customers_num;
    int** distances = distances_matrix_init(customers, customers_num);

    terminal_print_parameters(de_technique, NP, seed);

    differential_evolution(distances, customers, customers_num, vehicles_num, capacity_max, best_solution, de_technique); 
    
    distances = distances_matrix_free(distances, customers_num);

    return 0;
}
