#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>

#include "../common/local_search.h"
#include "../common/dependences.h"
#include "../common/io_tools.h"

#include "../metaheuristic/differential_evolution.h"

//double F, CR;
extern int NP;


Generation* generation_init() {
    static int id = 1;

    Generation *generation = (Generation*) malloc (sizeof(Generation));
    generation->individuals = (Individual**) malloc (NP * sizeof(Individual*));

/*  int i = 0;
    while (i < NP) {
        generation->individuals[i] = NULL;
        i++;
    }
*/
    generation->id = id;
    generation->best_solution = NULL;
    generation->feasible_solutions_num = 0;
    id++;
    
    return generation;
}


Generation* initial_population(int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max) {   
    Generation *generation = generation_init();
    Individual *individual;
    
    bool gen_has_feasible = false;
    enum IndividualType individual_type;
    
    int select;
    for (int i = 0; i < NP; i++) {
        select = rand() % 2;
        if (select == 1) {
            individual_type = TOP_TO_DOWN;
        } else {
            individual_type = DOWN_TO_TOP;
        }
        
        individual = individual_generate_random(distances, customers, customers_num, capacity_max, vehicles_num, individual_type);

        generation->individuals[i] = individual;

        if (individual->feasible) {
            generation->feasible_solutions_num++;

            if (!gen_has_feasible || individual->cost < generation->best_solution->cost) {
                generation->best_solution = individual;
                gen_has_feasible = true;
            }
        }
    }
    
    return generation;
}


void generation_clear_cloned_flags(Generation* generation) {
    for (int i = 0; i < NP; i++) {
        if (generation->individuals[i]->cloned) {
            generation->individuals[i]->cloned = 0;
        }
    }

    return;
}


Generation* new_generation(Generation* generation, int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max, enum DETechnique de_technique ) {
    Individual  *mutant = NULL,
                *trial = NULL,
                *target = NULL;

    enum CrossoverType crossover_type;
    enum MutationType mutation_type;

    switch (de_technique) {
        case RAND_1_BIN:
            mutation_type = MUTATION_RAND;
            crossover_type = CROSSOVER_BIN;
            break;

        case RAND_1_EXP:
            mutation_type = MUTATION_RAND;
            crossover_type = CROSSOVER_EXP;
            break;

        case BEST_1_BIN:
            mutation_type = MUTATION_BEST;
            crossover_type = CROSSOVER_BIN;
            break;

        case BEST_1_EXP:
            mutation_type = MUTATION_BEST;
            crossover_type = CROSSOVER_EXP;
            break;

        default:
            printf("[ERROR]: Bad DE technique.\n");
            exit(1);
    }
    
    Generation* generation2 = generation_init();
    generation2->best_solution = generation->best_solution;

    if (generation2->best_solution != NULL) {
        generation2->best_solution->cloned = 1;
    }

    int generation2_has_best_solution_generation1 = 0;

    for (int target_idx = 0; target_idx < NP; target_idx++) {
        mutant = mutation(generation, target_idx, customers_num, vehicles_num, mutation_type);
        target = generation->individuals[target_idx];
        trial = crossover(target, mutant, customers_num, vehicles_num, crossover_type);
        
        mutant = individual_free(mutant, vehicles_num);
        individual_reevaluate(trial, capacity_max, vehicles_num, distances, customers);

        local_search(trial, distances, customers, vehicles_num);
        
        if (trial->cost < target->cost) {
            if (trial->feasible) {
                generation2->individuals[target_idx] = trial;
                generation2->feasible_solutions_num++;

                if (generation2->best_solution != NULL && trial->cost < generation2->best_solution->cost) {   
                    if (generation2->best_solution->cloned) {
                        generation2->best_solution->cloned = 0;
                    }

                    generation2->best_solution = trial;
                }
                /* If the trial is infeasible and has a better cost over the last generation best feasible solution, it will NOT substitute the target. */
            } else if (target == generation2->best_solution && target->feasible) {
                generation2->individuals[target_idx] = target;
                target->cloned = 1;

                generation2->feasible_solutions_num++;
                generation2_has_best_solution_generation1 = 1;
                trial = individual_free(trial, vehicles_num);
                
            } else {
                generation2->individuals[target_idx] = trial;
            }

        } else {
            generation2->individuals[target_idx] = target;
            target->cloned = 1;
            trial = individual_free(trial, vehicles_num);
            
            if (target->feasible) {
                generation2->feasible_solutions_num++;
                
                if (target == generation2->best_solution) {
                    generation2_has_best_solution_generation1 = 1;
                }
            }
        }
    }

    if (generation2_has_best_solution_generation1)
        generation->best_solution->cloned = 1;
    
    return generation2;
}


/* If the individual has the cloned flag enabled, means that it was passed to the new generation */
Generation* generation_free(Generation* generation, int vehicles_num) {
    for (int i = 0; i < NP; i++) {
        if (!generation->individuals[i]->cloned) {
            generation->individuals[i] = individual_free(generation->individuals[i], vehicles_num);
        }
    }
    
    free(generation->individuals);
    free(generation);

    return NULL;
}


Individual* generate_new_mutant(Individual* x1, Individual* x2, Individual* x3, int vehicles_num, int customers_num) {
    int route_target = 0,
        index_target = 0,
        customer_target = 0,
        mutant_route_idx = 0,
        mutant_visitation_idx = 0,
        visitation_index = 0,
        customer_chosen = 0;

    int customers_possible[customers_num];
    int customers_possible_num = customers_num -1, //TODO: rethink name, i believe its counting with 0 (do/while reasons)
        random_idx = 0;
        
    for (int index = 0; index < customers_num; index++) {
        customers_possible[index] = index;
    }
        
    Individual* mutant = individual_make_hard_clone(x1, customers_num, vehicles_num);
    
    
    int pertubed_components_cnt = 0,
        pertubed_components_max = (customers_num/2.0) * F;
    do {
        random_idx = (rand() % customers_possible_num) +1;
        customer_chosen = customers_possible[random_idx];
        
        mutant_route_idx = x3->positions[0][customer_chosen];
        mutant_visitation_idx = x3->positions[1][customer_chosen];
        
        if (mutant->routes_end[mutant_route_idx] < mutant_visitation_idx +1) { /* A rota nao possui a quantidade de customers necessária. Entao a cidade será inserida ao fim da rota. */
            individual_remove_customer(mutant, customer_chosen, 0);

            /* Inserindo a cidade na rota. */
            visitation_index = mutant->routes_end[mutant_route_idx];

            mutant->routes[mutant_route_idx][visitation_index] = customer_chosen;
            mutant->positions[0][customer_chosen] = mutant_route_idx;
            mutant->positions[1][customer_chosen] = visitation_index;
            
            mutant->routes_end[mutant_route_idx]++;

        } else {
            customer_target = mutant->routes[mutant_route_idx][mutant_visitation_idx];
        
            route_target = mutant->positions[0][customer_chosen];
            index_target = mutant->positions[1][customer_chosen];
            
            mutant->routes[mutant_route_idx][mutant_visitation_idx] = customer_chosen;
            mutant->positions[0][customer_chosen] = mutant_route_idx;
            mutant->positions[1][customer_chosen] = mutant_visitation_idx;
            
            mutant->routes[route_target][index_target] = customer_target;
            mutant->positions[0][customer_target] = route_target;
            mutant->positions[1][customer_target] = index_target;
        }
        
        while (random_idx < customers_possible_num -1) {
            customers_possible[random_idx] = customers_possible[random_idx +1];
            random_idx++;
        } 
        customers_possible_num--;
        
        pertubed_components_cnt++;
    } while (pertubed_components_cnt < pertubed_components_max);

    return mutant;
}


/* @param mutation_type
 *   - 0 (MUTATION_RAND): the individual target_idx is the best of the generation
 *   - 1 (MUTATION_BEST): the individual target_idx is random
 */
Individual* mutation(Generation* generation, int target_idx, int customers_num, int vehicles_num, enum MutationType mutation_type) {
    int r1 = rand() % NP;
    int r2 = rand() % NP;
    int r3 = rand() % NP;

    while (r2 == target_idx) r2 = rand() % NP;
    
    while (r3 == target_idx || r3 == r2) r3 = rand() % NP;

    if (mutation_type == MUTATION_BEST && generation->best_solution != NULL) {
        return generate_new_mutant(generation->best_solution, generation->individuals[r2], generation->individuals[r3], vehicles_num, customers_num);
    }
    

    while (r1 == target_idx || r1 == r2 || r1 == r3) r1 = rand() % NP;

    return generate_new_mutant(generation->individuals[r1], generation->individuals[r2], generation->individuals[r3], vehicles_num, customers_num);
}


/* A cidade perturbada é a cidade da posicao antiga do individual, que será substituida pela cidade mutant. */
Individual* crossover(Individual* x1, Individual* mutant, int customers_num, int vehicles_num, enum CrossoverType crossover_type) {
    double random = 0.;
    
    int index = 0,
        trial_idx = 0,
        trial_route = 0,
        mutant_idx = 0,
        mutant_route_idx = 0,
        customer_target = 0;

    /* First permutation happens without evaluating CR */
    int j_rand_route = rand() % vehicles_num;
    while (mutant->routes_end[j_rand_route] == 0) {
        if (j_rand_route < vehicles_num -1) { /* Not necessary if all routes have a customer */
            j_rand_route++; 
        } else {
            j_rand_route = 0;
        }
    }
    int j_rand_component = rand() % mutant->routes_end[j_rand_route];
    
    /* A closed customer is a customer that was already perturbed. It is used to not make multiple swaps with same customer */
    int customers_closed[customers_num];
    memset(customers_closed, 0, customers_num*sizeof(int));
    customers_closed[0] = 1;
    
    Individual* trial = individual_make_hard_clone(x1, customers_num, vehicles_num);

    
    int customer_chosen = mutant->routes[j_rand_route][j_rand_component];
    
    /* Perturbando o individual com a cidade selecionada jrand, para garantir que o indivíduo trial seja diferente do indivíduo target_idx. */
    if (j_rand_component >= trial->routes_end[j_rand_route]) {      
        individual_remove_customer(trial, customer_chosen, 0);
        
        /* Adicionando na posicao j_rand. */
        index = trial->routes_end[j_rand_route];
        
        trial->routes[j_rand_route][index] = customer_chosen;
        trial->positions[0][customer_chosen] = j_rand_route; 
        trial->positions[1][customer_chosen] = index;
        
        trial->routes_end[j_rand_route]++;
        customers_closed[customer_chosen] = 1;
    } else {
        customer_target = trial->routes[j_rand_route][j_rand_component];    
    
        /* Se as customers forem iguais a perturbação não é efetuada. */
        if (customer_chosen == customer_target) { 
            customers_closed[customer_chosen] = 1;
        } else {
        
            mutant_route_idx = trial->positions[0][customer_chosen];
            mutant_idx = trial->positions[1][customer_chosen];
        
            trial->routes[j_rand_route][j_rand_component] = customer_chosen;
            trial->positions[0][customer_chosen] = j_rand_route;
            trial->positions[1][customer_chosen] = j_rand_component;
            customers_closed[customer_chosen] = 1;
            
            trial->routes[mutant_route_idx][mutant_idx] = customer_target;
            trial->positions[0][customer_target] = mutant_route_idx;
            trial->positions[1][customer_target] = mutant_idx;
            customers_closed[customer_target] = 1;
        }
    }
    
    for (int i = 0; i < vehicles_num; i++) {
        for (int j = 0; j < mutant->routes_end[i]; j++) {
            random = (double) rand() / RAND_MAX;
            
            if (random <= CR) {
                customer_chosen = mutant->routes[i][j];
                
                /* Se a cidade não foi fechada a perturbação será feita. */
                if (!customers_closed[customer_chosen]) {
                    if (j >= trial->routes_end[i]) {
                        /* Individual target_idx não possui cidade no mesmo indice da componente selecionada. Então a cidade será adicionada ao final daquela lista. */
                        individual_remove_customer(trial, customer_chosen, 0);
                    
                        /* Adicionando na posicao selecionada. */
                        index = trial->routes_end[i];
                        
                        trial->routes[i][index] = customer_chosen;
                        trial->positions[0][customer_chosen] = i; 
                        trial->positions[1][customer_chosen] = index;
        
                        trial->routes_end[i]++;
                        customers_closed[customer_chosen] = 1;
                    } else {
                        customer_target = trial->routes[i][j];
                        
                        /* Se as customers forem iguais a perturbação não é efetuada. */
                        if (customer_chosen == customer_target) { 
                            customers_closed[customer_chosen] = 1;
                        } else {
                    
                            trial_route = trial->positions[0][customer_target];
                            trial_idx = trial->positions[1][customer_target];
                            mutant_route_idx = trial->positions[0][customer_chosen];
                            mutant_idx = trial->positions[1][customer_chosen];
        
                            trial->routes[trial_route][trial_idx] = customer_chosen;
                            trial->positions[0][customer_chosen] = trial_route;
                            trial->positions[1][customer_chosen] = trial_idx;
                            customers_closed[customer_chosen] = 1;
                            
                            trial->routes[mutant_route_idx][mutant_idx] = customer_target;
                            trial->positions[0][customer_target] = mutant_route_idx;
                            trial->positions[1][customer_target] = mutant_idx;
                            customers_closed[customer_target] = 1;
                        }
                    }
                }
                
            } else if (crossover_type == CROSSOVER_EXP) {
                goto END_CROSSOVER; 
            }
        }
    }

END_CROSSOVER:
    return trial;
}


void differential_evolution(int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max, int best_solution, enum DETechnique de_technique) {
    Generation *generation = initial_population(distances, customers, customers_num, vehicles_num, capacity_max),
               *generation_new = NULL;
    
    FILE *file_solution = NULL,
         *file_report = NULL;
    
    if (PRINT_IN_FILE) {
        file_solution = fopen("solution.txt", "w");
        file_report = fopen("report.txt", "w");
        generation_print_report_in_file(file_report, generation);
        fprintf(file_report, "\n");
    } else {
        generation_print_report(generation);
        printf("\n");
    }
    
    int generations_cnt = 1,
        found_best_solution = 0,
        best_fitness = generation->best_solution->cost;
    do {
        generation_new = new_generation(generation, distances, customers, customers_num, vehicles_num, capacity_max, de_technique);
        
        if (best_fitness != generation_new->best_solution->cost) {
            if (PRINT_IN_FILE) {
                generation_print_report_in_file(file_report, generation_new);
                fprintf(file_report, "\n");
            } else {
                generation_print_report(generation_new);
                printf("\n");
            }
            
            best_fitness = generation_new->best_solution->cost;
        }
        
        generation = generation_free(generation, vehicles_num);
        generation = generation_new;
        generations_cnt++;
        
        generation_clear_cloned_flags(generation);
        
        if (generation->best_solution != NULL && generation->best_solution->cost == best_solution) {
            found_best_solution = 1;
        }
        
    } while (!found_best_solution && generations_cnt < MAX_GEN);
    

    if (PRINT_IN_FILE) {
        if (generations_cnt == MAX_GEN) {
            generation_print_report_in_file(file_report, generation);
            fprintf(file_report, "\n");
        }
    
        if (generation->best_solution != NULL) {
            individual_print_in_file(file_solution, generation->best_solution, vehicles_num);
        } else {
            fprintf(file_solution, "\nDid not find a feasible solution.\n");
        }
        
        printf("    File solution.txt was write on disk.\n");
        fclose(file_report);
        fclose(file_solution);
        
    } else {
        if (generations_cnt == MAX_GEN) {
            generation_print_report(generation);
            printf("\n");
        }
    
        if (generation->best_solution != NULL) {
            individual_print(generation->best_solution, vehicles_num);
        } else {
            printf("\nDid not find a feasible solution.\n");
        }
    }

    generation_clear_cloned_flags(generation);
    if (generation->best_solution->cloned) {
        individual_free(generation->best_solution, vehicles_num);
    }
    generation = generation_free(generation, vehicles_num);

    return;
}
