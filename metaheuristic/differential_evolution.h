#ifndef DIFFERENTIAL_EVOLUTION_H
#define DIFFERENTIAL_EVOLUTION_H

#include "../common/dependences.h"

#define F 0.1
#define CR 0.9
//#define NP 250
#define MAX_GEN 200000
#define PENALTY 100

#define PRINT_IN_FILE 0


enum MutationType { MUTATION_RAND, MUTATION_BEST };
enum CrossoverType { CROSSOVER_BIN, CROSSOVER_EXP };
enum DETechnique { RAND_1_BIN, RAND_1_EXP, BEST_1_BIN, BEST_1_EXP };

typedef struct generation Generation;

struct generation{
    int         id;
    Individual  **individuals;
    Individual  *best_solution;
    int         feasible_solutions_num;
};


Generation* generation_init();
            
Generation* initial_population(int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max);

Generation* new_generation(Generation* generation, int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max, enum DETechnique de_technique);

Generation* generation_free(Generation* generation, int vehicles_num);


void differential_evolution(int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max, int best_solution, enum DETechnique de_technique);

Individual* mutation(Generation* generation, int target_idx, int customers_num, int vehicles_num, enum MutationType mutation_type);

Individual* crossover(Individual* x1, Individual* mutant, int customers_num, int vehicles_num, enum CrossoverType crossover_type);


Individual* generate_new_mutant(Individual* x1, Individual* x2, Individual* x3, int vehicles_num, int customers_num);

//generate new mutant continuous


void generation_clear_cloned_flags(Generation* generation);


#endif /*DIFFERENTIAL_EVOLUTION_H*/
