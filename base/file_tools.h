#ifndef FILE_TOOLS_H
#define FILE_TOOLS_H

#include <stdio.h>

#include "dependences.h"
#include "../metaheuristic/differential_evolution.h"


int escolha_tecnica();

void file_customers_init(Customer* customers, FILE* file, int customers_num);

void file_update_customer_demand(Customer* customers, FILE* file, int customers_num);


void individual_print(Individual* individual, int vehicles_num, int customers_num);

void generation_print_report(Generation* generation);


void individual_print_in_file(FILE* file, Individual* individual, int vehicles_num, int customers_num);

void generation_print_report_in_file(FILE* file, Generation* generation);


//Apenas para testes.
void individual_test(Individual* individual, Customer* customers, int customers_num, int vehicles_num); 


#endif /* FILE_TOOLS_H */
