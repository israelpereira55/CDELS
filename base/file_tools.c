#include <stdio.h>
#include <stdlib.h>

#include "dependences.h"
#include "../metaheuristic/differential_evolution.h"


int escolha_tecnica() {
    int escolha = 0;

    do {
        printf( "=================================================================== \n"
            "           CDELS for CVRP\n"
            "===================================================================\n\n"
        
        "Escolha a técnica:\n" 
        "    1 - DE/rand/1/bin\n" 
        "    2 - DE/rand/1/exp\n" 
        "    3 - DE/best/1/bin\n" 
        "    4 - DE/best/1/exp\n" 
        "    Técnica: ");
    
        scanf("%d", &escolha);
        if (escolha < 1 || escolha > 4) {
            printf("    Escolha inválida!\n\n");
            escolha = -1;
        }
    } while (escolha == -1);
    
    printf("\n");
    return escolha;
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

void individual_print(Individual* individual, int vehicles_num, int customers_num) {

    if (individual->feasible) {
        printf("Individual feasible\n");
    } else  printf("Individual inviavel\n");
    
    printf("Custo: %d\n\n", individual->cost);
    
    int *route = NULL,
          route_end;
    
    int i, j;
    for (i = 0; i < vehicles_num; i++) {
        printf("Veiculo #%d: ", i+1);
        route_end = individual->routes_end[i];
        route = individual->routes[i];
        
        for (j = 0; j < route_end; j++)
            printf("%d ", route[j] +1);
        
        printf("\n");
    }
    return;
}

void individual_print_in_file(FILE* file, Individual* individual, int vehicles_num, int customers_num) {

    if (individual->feasible) {
        fprintf(file, "Individual feasible\n");
    } else  fprintf(file, "Individual inviavel\n");
    
    fprintf(file, "Custo: %d\n\n", individual->cost);
    
    int *route = NULL,
         route_end;

    int i, j;         
    for (i = 0; i < vehicles_num; i++) {
        fprintf(file, "Veiculo #%d: ", i+1);
        route_end = individual->routes_end[i];
        route = individual->routes[i];
        
        for (j = 0; j < route_end; j++)
            fprintf(file, "%d ", route[j] +1);
        
        fprintf(file, "\n");
    }
    return;
}

void generation_print_report(Generation* generation) {
    printf("Geração: %d\n", generation->id);

    if (generation->best_solution != NULL) {
        printf("Melhor solução viável: %d\n", generation->best_solution->cost);
    } else  printf("Melhor solução viável: VAZIO\n");
    
    printf("Número de soluções viáveis: %d.\n", generation->feasible_solutions_num);
    return;
}

void generation_print_report_in_file(FILE* file, Generation* generation) {
    fprintf(file, "Geração: %d\n", generation->id);

    if (generation->best_solution != NULL) {
        fprintf(file, "Melhor solução viável: %d\n", generation->best_solution->cost);
    } else  fprintf(file, "Melhor solução viável: VAZIO\n");
    
    fprintf(file, "Número de soluções viáveis: %d.\n", generation->feasible_solutions_num);
    return;
}

void individual_test(Individual* individual, Customer* customers, int customers_num, int vehicles_num) {
    individual_print(individual, vehicles_num, customers_num);
    printf("\n");
        
    int cidades_visitadas[customers_num];
    
    printf("Indices: ");
    
    int i = 1;
    while (i < customers_num +1) {
        cidades_visitadas[i] = 0;
        i++;
        
        printf("%d ", (i-2)%10 );
    }
    
    printf("\n\n");
    
    int j = 0;
    printf("Rotas:   ");
    for (i = 0; i < customers_num; i++) {
            printf("%d ", individual->positions[0][i]);
    } 
    
    printf("\nCidades: ");
    for (i = 0; i < customers_num; i++) {
        printf("%d ", individual->positions[1][i]);
    } 
    
    printf("\n\n");
    
    for (i = 0; i < vehicles_num; i++) {
        printf("FIM DA ROTA %d: %d\n", i+1, individual->routes_end[i]);
    }
    
    printf("\n");
    
    int  load;
    for (i = 0; i < vehicles_num; i++) {
        load = 0;
        for (j = 0; j < individual->routes_end[i]; j++)
            load += customers[individual->routes[i][j]].demand;
            
        printf("CARGA DA ROTA %d: %d\n", i, load);
    }
    
    
    for (i = 0; i < vehicles_num; i++) {
        for (j = 0; j < individual->routes_end[i]; j++) {
            cidades_visitadas[ individual->routes[i][j] ]++;
        }
    }
    
    int count = 0;
    for (i = 1; i < customers_num; i++) {
        if ( !cidades_visitadas[i]) {
            printf("A CIDADE %d ESTA FALTANDO.\n", i+1);
        }
        
        if (cidades_visitadas[i] > 1) {
            printf("COPIA DA CIDADE %d, %d vzs\n", i+1, cidades_visitadas[i]);
            
        }
        
        if (cidades_visitadas[i]) {
            count++;
        }
    }
    
    printf("QUANTIDADE DE CIDADES %d\n\n", count);
    
    for (i = 0; i < vehicles_num; i++) {    
        if (individual->routes_end[i] == 0) {
            printf("A ROTA %d ESTA VAZIA!\n", i);
        }
    }
    
    for (i = 1; i < customers_num; i++) {
        if ( individual->routes[ individual->positions[0][i] ][ individual->positions[1][i] ] != i) {
                printf("POSICAO DA CIDADE %d ERRADA!\n", i+1);
        }
    } 
    
    return;
}
