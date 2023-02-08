#include <stdio.h>
#include <stdlib.h>

#include "dependences.h"
#include "../metaheuristic/differential_evolution.h"


//TODO: update all io strategy to arguments...
enum DETechnique terminal_choose_de_technique() {
    int chosen_id = 0;

    printf( "=================================================================== \n"
            "                           CDELS for CVRP\n"
            "===================================================================\n\n"
    
            "DE technique:\n" 
            "    1 - DE/rand/1/bin\n" 
            "    2 - DE/rand/1/exp\n" 
            "    3 - DE/best/1/bin\n" 
            "    4 - DE/best/1/exp\n");
    
    do {
        printf("\nTechnique: ");
        if (1 != scanf("%d", &chosen_id)) {
            printf("[ERROR]: IO error.\n"); 
            exit(1); 
        }

        if (chosen_id < 1 || chosen_id > 4) {
            printf("Wrong option!\n");
            chosen_id = -1;
        }
    } while (chosen_id == -1);
    
    enum DETechnique de_technique = 0;
    switch (chosen_id){
        case 1:
            de_technique = RAND_1_BIN;
            break;

        case 2:
            de_technique = RAND_1_EXP;
            break;

        case 3:
            de_technique = BEST_1_BIN;
            break;

        case 4:
            de_technique = BEST_1_EXP;
            break;

        default:
            printf("[ERROR]: Bad DE technique.\n");
            exit(1);
    }

    return de_technique;
}


/* WIP */
void file_read_instance(File* file, Customers* customers, Header* header){

    if (4 != fscanf(file_read, "%d %d %d %d", &(header->vehicles_num), &(header.best_solution), &(header.customers_num), &(header.capacity_max))) {
        printf("[ERROR]: IO error.\n"); 
        exit(1); 
    }

    file_customers_init(customers, file_read, customers_num);
}


void file_update_customer_demand(Customer* customers, FILE* file, int customers_num) {
    int id = 0,
        demand = 0;

    for (int i = 0; i < customers_num; i++) {

        if (fscanf (file, "%d %d", &id, &demand) != 2) {
            printf("[ERROR]: IO error.\n"); 
            exit(1); 
        }

        customers[i].demand = demand;
    }
    
    return;
}


//TODO: read the standard file header
void file_customers_init(Customer* customers, FILE* file, int customers_num) {
    Customer* customer = NULL;
    
    int id = 0;
    double x = 0., y = 0.;

    for (int i = 0; i < customers_num; i++) {

        if(3 != fscanf (file, "%d %lf %lf", &id, &x, &y)){
            printf("[ERROR]: IO error.\n"); 
            exit(1); 
        } 

        customer = &customers[i];
        customer->id = id;
        customer->x = x;
        customer->y = y;
    }
    
    int lines_num = 2; //TODO: check
    char c = 0;
    while (lines_num > 0) {
        c = fgetc(file);
        if (c == '\n') {
            lines_num--;
        }
    }
    
    file_update_customer_demand(customers, file, customers_num);

    return;
}


void individual_print(Individual* individual, int vehicles_num) {

    if (individual->feasible) {
        printf("Feasible solution\n");
    } else {
        printf("Infeasible solution\n");
    }
    
    printf("Cost: %d\n\n", individual->cost);
    
    int *route = NULL,
         route_end = -1;
    
    for (int i = 0; i < vehicles_num; i++) {
        printf("Vehicle #%d: ", i+1);
        route_end = individual->routes_end[i];
        route = individual->routes[i];
        
        for (int j = 0; j < route_end; j++) {
            printf("%d ", route[j] +1);
        }
        
        printf("\n");
    }

    return;
}


void individual_print_in_file(FILE* file, Individual* individual, int vehicles_num) {

    if (individual->feasible) {
        fprintf(file, "Feasible solution\n");
    } else {
        fprintf(file, "Infeasible solution\n");
    }
    
    fprintf(file, "Cost: %d\n\n", individual->cost);
    
    int *route = NULL,
         route_end = -1;

    for (int i = 0; i < vehicles_num; i++) {
        fprintf(file, "Vehicle #%d: ", i+1);
        route_end = individual->routes_end[i];
        route = individual->routes[i];
        
        for (int j = 0; j < route_end; j++) {
            fprintf(file, "%d ", route[j] +1);
        }
        
        fprintf(file, "\n");
    }

    return;
}


void generation_print_report(Generation* generation) {
    printf("Generation: %d\n", generation->id);

    if (generation->best_solution != NULL) {
        printf("Best solution: %d\n", generation->best_solution->cost);
    } else {
        printf("Best solution: None\n");
    }
    
    printf("Number of feasible solutions: %d.\n", generation->feasible_solutions_num);

    return;
}


void generation_print_report_in_file(FILE* file, Generation* generation) {
    fprintf(file, "Generation: %d\n", generation->id);

    if (generation->best_solution != NULL) {
        fprintf(file, "Best solution: %d\n", generation->best_solution->cost);
    } else {
        fprintf(file, "Best solution: None\n");
    }
    
    fprintf(file, "Number of feasible solutions: %d.\n", generation->feasible_solutions_num);

    return;
}


void terminal_print_parameters(enum DETechnique de_technique, int NP, int seed) {
    printf("\nCDELS Parameters:\n");

    printf("- Technique: ");
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
            printf("[ERROR]: Bad DE technique.\n");
            exit(1);
    }

    printf("- NP: %d\n"
           "- F: %.1f\n"
           "- CR: %.1f\n"
           "- Penalty: %.2f\n"
           "- Seed: %d\n\n", NP, F, CR, PENALTY, seed);

    return;
}

//TODO: check all
void terminal_individual_test(Individual* individual, Customer* customers, int customers_num, int vehicles_num) {
    individual_print(individual, vehicles_num);
    printf("\n");
        
    int customers_visited[customers_num];
    
    printf("Indexes: ");
    
    int i = 1;
    while (i < customers_num +1) {
        customers_visited[i] = 0;
        i++;
        
        printf("%d ", (i-2)%10 );
    }
    
    printf("\n\n");
    
    int j = 0;
    printf("Routes:   ");
    for (i = 0; i < customers_num; i++) {
            printf("%d ", individual->positions[0][i]);
    } 
    
    printf("\nCustomers: ");
    for (i = 0; i < customers_num; i++) {
        printf("%d ", individual->positions[1][i]);
    } 
    
    printf("\n\n");
    
    for (i = 0; i < vehicles_num; i++) {
        printf("End of route %d: %d\n", i+1, individual->routes_end[i]);
    }
    
    printf("\n");
    
    int  load;
    for (i = 0; i < vehicles_num; i++) {
        load = 0;
        for (j = 0; j < individual->routes_end[i]; j++)
            load += customers[individual->routes[i][j]].demand;
            
        printf("Route load %d: %d\n", i, load);
    }
    
    
    for (i = 0; i < vehicles_num; i++) {
        for (j = 0; j < individual->routes_end[i]; j++) {
            customers_visited[ individual->routes[i][j] ]++;
        }
    }
    
    int count = 0;
    for (i = 1; i < customers_num; i++) {
        if ( !customers_visited[i]) {
            printf("[ERROR]: Customer %d is missing.\n", i+1);
        }
        
        if (customers_visited[i] > 1) {
            printf("[ERROR]: Copy of customer %d, %d times\n", i+1, customers_visited[i]);
            
        }
        
        if (customers_visited[i]) {
            count++;
        }
    }
    
    printf("Number of customers %d\n\n", count);
    
    for (i = 0; i < vehicles_num; i++) {    
        if (individual->routes_end[i] == 0) {
            printf("[ERROR]: Route %d is empty!\n", i);
        }
    }
    
    for (i = 1; i < customers_num; i++) {
        if ( individual->routes[ individual->positions[0][i] ][ individual->positions[1][i] ] != i) {
                printf("[ERROR]: Position of customer %d is wrong!\n", i+1);
        }
    } 
    
    return;
}
