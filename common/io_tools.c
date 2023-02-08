#include <stdio.h>
#include <stdlib.h>

#include "dependences.h"
#include "../metaheuristic/differential_evolution.h"


//TODO: update all io strategy to arguments...
enum DETechnique terminal_choose_de_technique() {
    int chosen_id = 0;
    
    do {
        printf( "=================================================================== \n"
            "           CDELS for CVRP\n"
            "===================================================================\n\n"
        
        "Choose the DE technique:\n" 
        "    1 - DE/rand/1/bin\n" 
        "    2 - DE/rand/1/exp\n" 
        "    3 - DE/best/1/bin\n" 
        "    4 - DE/best/1/exp\n" 
        "    Technique: ");
    
        scanf("%d", &chosen_id);
        if (chosen_id < 1 || chosen_id > 4) {
            printf("    Wrong option!\n\n");
            chosen_id = -1;
        }
    } while (chosen_id == -1);
    printf("\n");
    
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
            de_technique = RAND_1_BIN;
    }

    return de_technique;
}

void file_update_customer_demand(Customer* customers, FILE* file, int customers_num) {
    int id,
        demand;
        
    for (int i = 0; i < customers_num; i++) {
        fscanf (file, "%d %d", &id, &demand);
        customers[i].demand = demand;
    }
    
    return;
}

//TODO: read the standard file header
void file_customers_init(Customer* customers, FILE* file, int customers_num) {
    Customer* customer = NULL;
    
    double x, y;
    int i, id;
    for (i = 0; i < customers_num; i++) {
        fscanf (file, "%d %lf %lf", &id, &x, &y);  
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
        printf("Individual is feasible\n");
    } else  printf("Individual is infeasible\n");
    
    printf("Cost: %d\n\n", individual->cost);
    
    int *route = NULL,
          route_end;
    
    int i, j;
    for (i = 0; i < vehicles_num; i++) {
        printf("Vehicle #%d: ", i+1);
        route_end = individual->routes_end[i];
        route = individual->routes[i];
        
        for (j = 0; j < route_end; j++)
            printf("%d ", route[j] +1);
        
        printf("\n");
    }
    return;
}

void individual_print_in_file(FILE* file, Individual* individual, int vehicles_num) {

    if (individual->feasible) {
        fprintf(file, "Individual is feasible\n");
    } else  fprintf(file, "Individual is infeasible\n");
    
    fprintf(file, "Cost: %d\n\n", individual->cost);
    
    int *route = NULL,
         route_end;

    int i, j;         
    for (i = 0; i < vehicles_num; i++) {
        fprintf(file, "Vehicle #%d: ", i+1);
        route_end = individual->routes_end[i];
        route = individual->routes[i];
        
        for (j = 0; j < route_end; j++)
            fprintf(file, "%d ", route[j] +1);
        
        fprintf(file, "\n");
    }
    return;
}

void generation_print_report(Generation* generation) {
    printf("Generation: %d\n", generation->id);

    if (generation->best_solution != NULL) {
        printf("Best feasible solution: %d\n", generation->best_solution->cost);
    } else  printf("Best feasible solution: VAZIO\n");
    
    printf("Number of feasible solutions: %d.\n", generation->feasible_solutions_num);
    return;
}

void generation_print_report_in_file(FILE* file, Generation* generation) {
    fprintf(file, "Generation: %d\n", generation->id);

    if (generation->best_solution != NULL) {
        fprintf(file, "Best feasible solution: %d\n", generation->best_solution->cost);
    } else  fprintf(file, "Best feasible solution: VAZIO\n");
    
    fprintf(file, "Number of feasible solutions: %d.\n", generation->feasible_solutions_num);
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