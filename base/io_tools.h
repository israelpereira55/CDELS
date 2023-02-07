#ifndef IO_TOOLS_H
#define IO_TOOLS_H

#include <stdio.h>

#include "dependences.h"
#include "../metaheuristic/differential_evolution.h"


enum DETechnique terminal_choose_de_technique();

void file_customers_init(Customer* customers, FILE* file, int customers_num);

void file_update_customer_demand(Customer* customers, FILE* file, int customers_num);


void individual_print(Individual* individual, int vehicles_num);

void generation_print_report(Generation* generation);


void individual_print_in_file(FILE* file, Individual* individual, int vehicles_num);

void generation_print_report_in_file(FILE* file, Generation* generation);


void terminal_individual_test(Individual* individual, Customer* customers, int customers_num, int vehicles_num); 


#endif /* FILE_TOOLS_H */
