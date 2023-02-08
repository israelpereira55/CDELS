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
    //F = 0.1;
    //CR = 0.9;
    int seed = time(NULL);
    
    int customers_num = 0,
        vehicles_num = 0,
        best_solution = 0,
        capacity_max = 0;

    enum DETechnique de_technique = terminal_choose_de_technique();
        
    switch (de_technique) {
        case RAND_1_BIN:
            printf("DE/rand/1/bin\n");
            break;

        case RAND_1_EXP:
            printf("DE/rand/1/exp\n");
            break;

        case BEST_1_BIN:
            printf("DE/best/1/bin\n");
            break;
        
        case BEST_1_EXP:
            printf("DE/best/1/exp\n");
            break;


        default:
            printf("ERROR?\n");
            exit(1);
    }

    char filename[81];

    printf("    Name of the instance file: ");
    scanf("%s", filename);

    FILE* file_read = fopen(filename, "r");
    if (file_read == NULL) {
        printf("    File %s was not found.\n    Aborting...\n\n", filename);
        return 0;
    }
    
    fscanf(file_read, "%d %d %d %d", &vehicles_num, &best_solution, &customers_num, &capacity_max);
    
    Customer customers[customers_num];
    file_customers_init(customers, file_read, customers_num);

    fclose(file_read);

    int** distances = distances_matrix_init(customers, customers_num);

    
    printf("F=%.2lf, CR=%.2lf, Penalty=%d\n", F, CR, PENALTY);

    NP = 3 * customers_num;
    printf("NP=%d\n", NP);
    
    printf("Seed: %d\n\n", seed); 
    srand( seed );
    
    differential_evolution(distances, customers, customers_num, vehicles_num, capacity_max, best_solution, de_technique); 
    
    distances = distances_matrix_free(distances, customers_num);
    return 0;
}
