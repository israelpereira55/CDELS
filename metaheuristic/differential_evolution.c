#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include "../base/local_search.h"
#include "../base/dependences.h"
#include "../base/arquivo.h"

#include "../metaheuristic/differential_evolution.h"

//double F, CR;
//int NP;

Generation* generation_init() {
	static int id = 1;

	Generation *generation = (Generation*) malloc (sizeof(Generation));
	generation->individuals = (Individual**) malloc (NP * sizeof(Individual*));

/*	int i = 0;
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
	
	//TODO: remove and include in loop
	int select = rand() % 2;
	if (select == 1) {
		individual = individual_generate_top_to_down(distances, customers, customers_num, capacity_max, vehicles_num);
	} else {
		individual = individual_generate_down_to_top(distances, customers, customers_num, capacity_max, vehicles_num);
	}

	generation->individuals[0] = individual;
	generation->best_solution = individual;
	
	if (individual->feasible)
		generation->feasible_solutions_num++;
	
	int i = 1;
	while (i < NP) {
		select = rand() % 2;
		if (select == 1) {
			individual = individual_generate_top_to_down(distances, customers, customers_num, capacity_max, vehicles_num);
		} else {
			individual = individual_generate_down_to_top(distances, customers, customers_num, capacity_max, vehicles_num);
		}

		generation->individuals[i] = individual;

		if (individual->feasible) {
			generation->feasible_solutions_num++;
			if (individual->cost < generation->best_solution->cost)
				generation->best_solution = individual;
		}
		i++;
	}
	
	return generation;
}

void generation_clear_cloned_flags(Generation* generation) {
	int i;
	for (i = 0; i < NP; i++) {
		if (generation->individuals[i]->cloned) {
			generation->individuals[i]->cloned = 0;
		}
	}
}

Generation* new_generation(Generation* generation, int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max, int mutation_rand, int crossover_bin) {
	Individual	*mutant = NULL,
				*trial = NULL,
				*target = NULL;
	
	Generation* generation2 = generation_init();
	generation2->best_solution = generation->best_solution;

	if (generation2->best_solution != NULL)
		generation2->best_solution->cloned = 1;

	int generation2_has_best_solution_generation1 = 0;
	
	int target_idx = 0;
	while (target_idx < NP) {
		mutant = mutation(generation, target_idx, customers, customers_num, vehicles_num, mutation_rand);
		target = generation->individuals[target_idx];
		trial = crossover(target, mutant, crossover_bin, customers_num, vehicles_num);
		
		mutant = individual_free(mutant, vehicles_num);
		individual_reevaluate(trial, capacity_max, vehicles_num, distances, customers);

		local_search(trial, distances, customers, customers_num, vehicles_num, capacity_max);
		
		if (trial->cost < target->cost) {
			if (trial->feasible) {
				generation2->feasible_solutions_num++;
				if (generation2->best_solution != NULL) {
					if (trial->cost < generation2->best_solution->cost) {	
				
						if (generation2->best_solution->cloned) {
							generation2->best_solution->cloned = 0;
						}
						generation2->best_solution = trial;
					}
				}
				
				generation2->individuals[target_idx] = trial;
				
			/* Se o trial for inviável mas tiver cost melhor que o melhor individual, ele nao substituirá o target_idx (best). */	
			} else if (target == generation2->best_solution) {
				generation2->individuals[target_idx] = target;
				target->cloned = 1;
				generation2->feasible_solutions_num++;
				generation2_has_best_solution_generation1 = 1;
				trial = individual_free(trial, vehicles_num);
				
			} else generation2->individuals[target_idx] = trial;

		} else {
			generation2->individuals[target_idx] = target;
			target->cloned = 1;
			trial = individual_free(trial, vehicles_num);
			
			if (target->feasible) {
				generation2->feasible_solutions_num++;
				
				if (target == generation2->best_solution)
					generation2_has_best_solution_generation1 = 1;
			}
		}
		target_idx++; //TODO: can go to a for
	}

	if (generation2_has_best_solution_generation1)
		generation->best_solution->cloned = 1;
	
	return generation2;
}

/* If the individual has the cloned flag enabled, means that it was passed to the new generation */
Generation* generation_free(Generation* generation, int vehicles_num) {
	int i;

	for (i = 0; i < NP; i++) {
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
	    customer_chosen = 0,
	    customers_possible[customers_num];
	    
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

// TODO: use enum
/* mutation_type = 0 o individual target_idx é a melhor solucao da populacao.
 * mutation_type = 1 o individual target_idx é aleatório.
 */
Individual* mutation(Generation* generation, int target_idx, Customer* customers, int customers_num, int vehicles_num, int mutation_type) {
	int r1 = rand() % NP;
	int r2 = rand() % NP;
	int r3 = rand() % NP;

	while (r2 == target_idx) 
		r2 = rand() % NP;
	
	while (r3 == target_idx || r3 == r2) 
		r3 = rand() % NP;

	if (mutation_type == 0 && generation->best_solution != NULL)
		return generate_new_mutant(generation->best_solution, generation->individuals[r2], generation->individuals[r3], vehicles_num, customers_num);
	
	while (r1 == target_idx || r1 == r2 || r1 == r3) 
		r1 = rand() % NP;
	
	return generate_new_mutant(generation->individuals[r1], generation->individuals[r2], generation->individuals[r3], vehicles_num, customers_num);
}

/* A cidade perturbada é a cidade da posicao antiga do individual, que será substituida pela cidade mutant. */
Individual* crossover(Individual* x1, Individual* mutant, int crossover_bin, int customers_num, int vehicles_num) {
	double random = 0.;
	
	int i = 0, j = 0,
	    index = 0,
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
		} else  j_rand_route = 0;
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
	
	for (i = 0; i < vehicles_num; i++) {
		for (j = 0; j < mutant->routes_end[i]; j++) {
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
				
			} else if (!crossover_bin) {
				goto FINALIZA_CROSSOVER; 
			}
		}
	}

FINALIZA_CROSSOVER:
	return trial;
}

void differential_evolution(int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max, int best_solution, int mutation_rand, int crossover_bin) {
	Generation *generation = initial_population(distances, customers, customers_num, vehicles_num, capacity_max),
	        *generation2 = NULL;
	
	FILE *file_solucao = NULL,
	     *relatorio = NULL;
	
	if (PRINT_IN_FILE) {
		file_solucao = fopen("solucao.vrp", "w");
		relatorio = fopen("relatorio.ed", "w");
		imprime_relatorio_arquivo(relatorio, generation);
		fprintf(relatorio, "\n");
	} else {
		imprime_relatorio_terminal(generation);
		printf("\n");
	}
	
	int generations_cnt = 1,
	    found_best_solution = 0,
	    best_fitness = generation->best_solution->cost;
	do {
		generation2 = new_generation(generation, distances, customers, customers_num, vehicles_num, capacity_max, mutation_rand, crossover_bin);
		
		if (best_fitness != generation2->best_solution->cost) {
			if (PRINT_IN_FILE) {
				imprime_relatorio_arquivo(relatorio, generation2);
				fprintf(relatorio, "\n");
			} else {
				imprime_relatorio_terminal(generation2);
				printf("\n");
			}
			
			best_fitness = generation2->best_solution->cost;
		}
		
		generation = generation_free(generation, vehicles_num);
		generation = generation2;
		generations_cnt++;
		
		generation_clear_cloned_flags(generation);
		
		if (generation->best_solution != NULL && generation->best_solution->cost == best_solution) {
			found_best_solution = 1;
		}
		
	} while (!found_best_solution && generations_cnt < MAX_GEN);
	

	if (PRINT_IN_FILE) {
		if (generations_cnt == MAX_GEN) {
			imprime_relatorio_arquivo(relatorio, generation2);
			fprintf(relatorio, "\n");
		}
	
		if (generation->best_solution != NULL) {
			imprime_individual_arquivo(file_solucao, generation->best_solution, vehicles_num, customers_num);
		} else  fprintf(file_solucao, "\nNão houve solução viável.\n");
		
		printf("    Verifique o arquivo solucao.vrp no disco.\n");
		fclose(relatorio);
		fclose(file_solucao);
		
	} else {
		if (generations_cnt == MAX_GEN) {
			imprime_relatorio_terminal(generation2);
			printf("\n");
		}
	
		if (generation->best_solution != NULL) {
			imprime_individual_terminal(generation->best_solution, vehicles_num, customers_num);
		} else  printf("\nNão houve solução viável.\n");
	}

	generation_clear_cloned_flags(generation);
	if (generation2->best_solution->cloned) {
		individual_free(generation2->best_solution, vehicles_num);
	}
	generation = generation_free(generation, vehicles_num);
	return;
}
