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

    Header header;
    file_read_header(file_read, &header);
    //printf("%d %d %d %d\n", header.vehicles_num, header.best_solution_value, header.customers_num, header.capacity_max);
    
    Customer customers[header.customers_num];
    file_customers_init(customers, file_read, header.customers_num);
    fclose(file_read);

    NP = 3 * header.customers_num;
    int** distances = distances_matrix_init(customers, header.customers_num);

    terminal_print_parameters(de_technique, NP, seed);

    differential_evolution(distances, customers, header.customers_num, header.vehicles_num, header.capacity_max, header.best_solution_value, de_technique); 
    
    distances = distances_matrix_free(distances, header.customers_num);

    return 0;
}
