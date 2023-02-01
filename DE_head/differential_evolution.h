#ifndef DIFFERENTIAL_EVOLUTION_H
#define DIFFERENTIAL_EVOLUTION_H

#include "../base/dependencies.h"

#define F 0.1
#define CR 0.9
#define NP 250
#define MAX_GEN 200000
#define PENALTY 100

#define PRINT_IN_FILE 0


enum MutationType { MUTATION_RAND, MUTATION_BEST };
enum CrossoverType { CROSSOVER_BIN, CROSSOVER_EXP };

typedef struct generation Generation;

struct generation{
	int 		id;
	Individual	**individuals;
	Individual	*best_solution;
	int			feasible_solutions_num;
};


Generation* generation_init();
			
Generation* initial_population(int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max);

Generation* new_generation(Generation* generation, int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max, int mutation_rand, int crossover_bin);

Generation* generation_free(Generation* generation, int vehicles_num);


void differential_evolution_1(int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max, int best_solution, int mutation_rand, int crossover_bin);

void differential_evolution_2(Customer* customers, int customers_num, int vehicles_num, int capacity_max, int best_solution, int mutation_rand, int crossover_bin);

void differential_evolution_2_rand_to_best(Customer* customers, int customers_num, int vehicles_num, int capacity_max, int best_solution, int crossover_bin);


Individual* generate_new_mutant(Individual* x1, Individual* x2, Individual* x3, int vehicles_num, int customers_num);

Individual* mutation(Generation* generation, int target_idx, Customer* customers, int customers_num, int vehicles_num, int mutation_type);

Individual* crossover(Individual* x1, Individual* mutant, int crossover_bin, int customers_num, int vehicles_num);


#endif /*DIFFERENTIAL_EVOLUTION_H*/
