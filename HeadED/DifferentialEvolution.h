#ifndef DIFFERENTIAL_EVOLUTION_H_
#define DIFFERENTIAL_EVOLUTION_H_

#include "../Common/dependencies.h"

#define F 0.1
#define CR 0.9
#define NP 250
#define MAX_GEN 200000
#define PENALTY 100

#define PRINT_IN_FILE 0


typedef struct generation Generation;

struct generation{
	int id;
	Individual** individuals;
	Individual* best_solution;
	int num_feasible_solutions;
};

Generation* generation_init();
			
Generation* initial_population(int** distances, Customer* customers, int num_customers, int num_vehicles, int max_capacity);

Generation* new_generation(Generation* generation, int** distances, Customer* customers, int num_customers, int num_vehicles, int max_capacity, int mutation_rand, int crossover_bin);

Generation* generation_free(Generation* generation, int num_vehicles);


void DifferentialEvolution_1(int** distances, Customer* customers, int num_customers, int num_vehicles, int max_capacity, int best_solution, int rand, int crossover_bin);

void DifferentialEvolution_2(Customer* customers, int num_customers, int num_vehicles, int max_capacity, int best_solution, int rand, int crossover_bin);

void DifferentialEvolution_2_rand_to_best(Customer* customers, int num_customers, int num_vehicles, int max_capacity, int best_solution, int crossover_bin);


Individual* individual_generate_mutant(Individual* x1, Individual* x2, Individual* x3, int num_vehicles, int num_customers);

Individual* mutation(Generation* generation, int target, Customer* customers, int num_customers, int num_vehicles, int mutation_type);

Individual* crossover(Individual* x1, Individual* mutant, int crossover_bin, int num_customers, int num_vehicles);


#endif /*EVOLUCAO_DIFERENCIAL_H_*/
