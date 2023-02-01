#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include "../Base/BuscaLocal.h"
#include "../Base/dependencias.h"
#include "../Base/arquivo.h"

#include "../HeadED/EvolucaoDiferencial.h"

//double F, CR;
//int NP;

Generation* generation_init() {
	static int id = 1;

	Generation *generation = (Generation*) malloc (sizeof(Generation));
	generation->individuals = (Individual**) malloc (NP * sizeof(Individual*));

/*	int i = 0;
	while(i < NP) {
		generation->individuals[i] = NULL;
		i++;
	}
*/
	generation->id = id;
	generation->best_solution = NULL;
	generation->num_feasible_solutions = 0;
	id++;
	
	return generation;
}

Generation* initial_population(int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max) {	
	Generation *generation = generation_init();
	Individual *individual;
	
	//TODO: remove and include in loop
	int select = rand() % 2;
	if(select == 1) {
		individual = individual_generate_top_to_down(distances, customers, customers_num, capacity_max, vehicles_num);
	} else {
		individual = individual_generate_down_to_top(distances, customers, customers_num, capacity_max, vehicles_num);
	}

	generation->individuals[0] = individual;
	generation->best_solution = individual;
	
	if(individual->feasible)
		generation->num_feasible_solutions++;
	
	int i = 1;
	while(i < NP) {
		select = rand() % 2;
		if(select == 1) {
			individual = individual_generate_top_to_down(distances, customers, customers_num, capacity_max, vehicles_num);
		} else {
			individual = individual_generate_down_to_top(distances, customers, customers_num, capacity_max, vehicles_num);
		}

		generation->individuals[i] = individual;

		if(individual->feasible) {
			generation->num_feasible_solutions++;
			if(individual->cost < generation->best_solution->cost)
				generation->best_solution = individual;
		}
		i++;
	}
	
	return generation;
}

void generation_zera_clones(Generation* generation){
	int i;
	for(i = 0; i < NP; i++){
		if(generation->individuals[i]->cloned){
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

	if(generation2->best_solution != NULL)
		generation2->best_solution->cloned = 1;

	int generation2_has_best_solution_generation1 = 0;
	
	int target_index = 0;
	while(target_index < NP) {
		mutante = mutation(generation, target_index, customers, customers_num, vehicles_num, mutation_rand);
		target = generation->individuals[target_index];
		trial = crossover(target, mutant, crossover_bin, customers_num, vehicles_num);
		
		mutante = individual_free(mutante, vehicles_num);
		individual_update_attributes(trial, capacity_max, vehicles_num, distances, customers);

		LocalSearch(trial, distances, customers, customers_num, vehicles_num, capacity_max);
		
		if(trial->cost < target->cost){
			if(trial->feasible){
				generation2->num_feasible_solutions++;
				if(generation2->best_solution != NULL){
					if(trial->cost < generation2->best_solution->cost){	
				
						if(generation2->best_solution->cloned){
							generation2->best_solution->cloned = 0;
						}
						generation2->best_solution = trial;
					}
				}
				
				generation2->individuals[target_index] = trial;
				
			/* Se o trial for inviável mas tiver cost melhor que o melhor individual, ele nao substituirá o target_index (best). */	
			} else if(target == generation2->best_solution){
				generation2->individuals[target_index] = target;
				target->cloned = 1;
				generation2->num_feasible_solutions++;
				generation2_has_best_solution_generation1 = 1;
				trial = individual_free(trial, vehicles_num);
				
			} else generation2->individuals[target_index] = trial;

		} else {
			generation2->individuals[target_index] = target;
			target->cloned = 1;
			trial = individual_libera(trial, vehicles_num);
			
			if(target->feasible){
				generation2->num_feasible_solutions++;
				
				if(target == generation2->best_solution)
					generation2_possui_best_solution_generation1 = 1;
			}
		}
		target_index++;
	}

	if(generation2_has_best_solution_generation1)
		generation->best_solution->cloned = 1;
	
	return generation2;
}

Generation* generation_free(Generation* generation, int vehicles_num) {
	int i;

	for(i = 0; i < NP; i++) {
		if(!generation->individuals[i]->cloned)
			generation->individuals[i] = individual_free(generation->individuals[i], vehicles_num);
	}
	
	free(generation->individuals);
	free(generation);
	return NULL;
}

Individual* generate_new_mutant(Individual* x1, Individual* x2, Individual* x3, int vehicles_num, int customers_num) {
	int rota_alvo = 0,
	    index_alvo = 0,
	    cidade_alvo = 0,
	    rota_mutante = 0,
	    index_mutante = 0,
	    cidade_escolhida = 0,
	    cidades_possiveis[customers_num];
	    
	int num_cidades_possiveis = customers_num -1,
	    index_cidade_selecionada = 0;
	    
	int index;
	for(index = 0; index < customers_num; index++){
		cidades_possiveis[index] = index;
	}
	    
	Individual* mutante = individual_clona(x1, customers_num, vehicles_num);
	
	int **rotas = mutante->rotas,
	    *fim_rotas = mutante->fim_rotas,
	    *posicoes_rotas = mutante->posicoes[0],
	    *posicoes_cidades = mutante->posicoes[1];
	    
	int *x3_posicoes_rotas = x3->posicoes[0],
	    *x3_posicoes_cidades = x3->posicoes[1];
	
	int qtd_componentes_perturbadas_atuais = 0,
	    qtd_componentes_perturbadas = (customers_num/2.0) * F;
	do {
		index_cidade_selecionada = (rand() % num_cidades_possiveis) +1;
		cidade_escolhida = cidades_possiveis[index_cidade_selecionada];
		
		rota_mutante = x3_posicoes_rotas[cidade_escolhida];
		index_mutante = x3_posicoes_cidades[cidade_escolhida];
		
		if(fim_rotas[rota_mutante] < index_mutante +1){ /* A rota nao possui a quantidade de customers necessária. Entao a cidade será inserida ao fim da rota. */
			individual_remove_cidade(mutante, cidade_escolhida, 0);

			/* Inserindo a cidade na rota. */
			index = fim_rotas[rota_mutante];

			rotas[rota_mutante][index] = cidade_escolhida;
			posicoes_rotas[cidade_escolhida] = rota_mutante;
			posicoes_cidades[cidade_escolhida] = index;
			
			fim_rotas[rota_mutante]++;
		} else {
			cidade_alvo = rotas[rota_mutante][index_mutante];
		
			rota_alvo = posicoes_rotas[cidade_escolhida];
			index_alvo = posicoes_cidades[cidade_escolhida];
			
			rotas[rota_mutante][index_mutante] = cidade_escolhida;
			posicoes_rotas[cidade_escolhida] = rota_mutante;
			posicoes_cidades[cidade_escolhida] = index_mutante;
			
			rotas[rota_alvo][index_alvo] = cidade_alvo;
			posicoes_rotas[cidade_alvo] = rota_alvo;
			posicoes_cidades[cidade_alvo] = index_alvo;
		}
		
		while(index_cidade_selecionada < num_cidades_possiveis -1){
			cidades_possiveis[index_cidade_selecionada] = cidades_possiveis[index_cidade_selecionada +1];
			index_cidade_selecionada++;
		} 
		num_cidades_possiveis--;
		
		qtd_componentes_perturbadas_atuais++;
	}while (qtd_componentes_perturbadas_atuais < qtd_componentes_perturbadas);

	return mutante;
}

/* tipo_mutacao = 0 o individual target_index é a melhor solucao da populacao.
 * tipo_mutacao = 1 o individual target_index é aleatório.
 */
Individual* mutation(Generation* generation, int target_idx, Customer* customers, int customers_num, int vehicles_num, int mutation_type) {
	int r1 = rand() % NP;
	int r2 = rand() % NP;
	int r3 = rand() % NP;

	while(r2 == target_index) 
		r2 = rand() % NP;
	
	while(r3 == target_index || r3 == r2) 
		r3 = rand() % NP;

	if(tipo_mutacao == 0 && generation->best_solution != NULL)
		return individual_gera_mutante(generation->best_solution, generation->individuals[r2], generation->individuals[r3], vehicles_num, customers_num);
	
	while(r1 == target_index || r1 == r2 || r1 == r3) 
		r1 = rand() % NP;
	
	return individual_gera_mutante(generation->individuals[r1], generation->individuals[r2], generation->individuals[r3], vehicles_num, customers_num);
}

/* A cidade perturbada é a cidade da posicao antiga do individual, que será substituida pela cidade mutante. */
Individual* crossover(Individual* x1, Individual* mutant, int crossover_bin, int customers_num, int vehicles_num) {
	int *mutante_fim_rotas = mutante->fim_rotas;
	
	int j_rand_rota = rand() % vehicles_num;
	while(mutante_fim_rotas[j_rand_rota] == 0) {
		if(j_rand_rota < vehicles_num -1) {
			j_rand_rota++; 
		} else  j_rand_rota = 0;
	}
	
	int j_rand_componente = rand() % mutante_fim_rotas[j_rand_rota];
	
	int cidades_fechadas[customers_num];
	memset(cidades_fechadas, 0, customers_num*sizeof(int));
	cidades_fechadas[0] = 1;
	
	Individual* trial = individual_clona(x1, customers_num, vehicles_num);
	int* perturbado_fim_rotas = trial->fim_rotas;

	double random;
	int i, j,
	    index,
	    index_mutante,
	    index_perturbado,
	    rota_mutante,
	    rota_perturbada;
	
	int **rotas_mutantes = mutante->rotas,
	    **rotas_perturbadas = trial->rotas,
	    *posicoes_perturbadas_rotas = trial->posicoes[0],
	    *posicoes_perturbadas_cidades = trial->posicoes[1];
	    
	int cidade_mutante = rotas_mutantes[j_rand_rota][j_rand_componente];
	int cidade_perturbada;
	
	/* Perturbando o individual com a cidade selecionada jrand, para garantir que o indivíduo trial seja diferente do indivíduo target_index. */
	if(j_rand_componente >= perturbado_fim_rotas[j_rand_rota]) {		
		individual_remove_cidade(trial, cidade_mutante, 0);
		
		/* Adicionando na posicao j_rand. */
		index = perturbado_fim_rotas[j_rand_rota];
		
		rotas_perturbadas[j_rand_rota][index] = cidade_mutante;
		posicoes_perturbadas_rotas[cidade_mutante] = j_rand_rota; 
		posicoes_perturbadas_cidades[cidade_mutante] = index;
		
		perturbado_fim_rotas[j_rand_rota]++;
		cidades_fechadas[cidade_mutante] = 1;
	} else {
		cidade_perturbada = rotas_perturbadas[j_rand_rota][j_rand_componente];	
	
		/* Se as customers forem iguais a perturbação não é efetuada. */
		if(cidade_mutante == cidade_perturbada) { 
			cidades_fechadas[cidade_mutante] = 1;
		} else {
		
			rota_mutante = posicoes_perturbadas_rotas[cidade_mutante];
			index_mutante = posicoes_perturbadas_cidades[cidade_mutante];
		
			rotas_perturbadas[j_rand_rota][j_rand_componente] = cidade_mutante;
			posicoes_perturbadas_rotas[cidade_mutante] = j_rand_rota;
			posicoes_perturbadas_cidades[cidade_mutante] = j_rand_componente;
			cidades_fechadas[cidade_mutante] = 1;
			
			rotas_perturbadas[rota_mutante][index_mutante] = cidade_perturbada;
			posicoes_perturbadas_rotas[cidade_perturbada] = rota_mutante;
			posicoes_perturbadas_cidades[cidade_perturbada] = index_mutante;
			cidades_fechadas[cidade_perturbada] = 1;
		}
	}
	
	for(i = 0; i < vehicles_num; i++){
		for(j = 0; j < mutante->fim_rotas[i]; j++){
			random = (double) rand() / RAND_MAX;
			
			if(random <= CR) {
				cidade_mutante = rotas_mutantes[i][j];
				
				/* Se a cidade não foi fechada a perturbação será feita. */
				if(!cidades_fechadas[cidade_mutante]){
					if(j >= perturbado_fim_rotas[i]){
						/* Individual target_index não possui cidade no mesmo indice da componente selecionada. Então a cidade será adicionada ao final daquela lista. */
						individual_remove_cidade(trial, cidade_mutante, 0);
					
						/* Adicionando na posicao selecionada. */
						index = perturbado_fim_rotas[i];
						
						rotas_perturbadas[i][index] = cidade_mutante;
						posicoes_perturbadas_rotas[cidade_mutante] = i; 
						posicoes_perturbadas_cidades[cidade_mutante] = index;
		
						perturbado_fim_rotas[i]++;
						cidades_fechadas[cidade_mutante] = 1;
					} else {
						cidade_perturbada = rotas_perturbadas[i][j];
						
						/* Se as customers forem iguais a perturbação não é efetuada. */
						if(cidade_mutante == cidade_perturbada) { 
							cidades_fechadas[cidade_mutante] = 1;
						} else {
					
							rota_perturbada = posicoes_perturbadas_rotas[cidade_perturbada];
							index_perturbado = posicoes_perturbadas_cidades[cidade_perturbada];
							rota_mutante = posicoes_perturbadas_rotas[cidade_mutante];
							index_mutante = posicoes_perturbadas_cidades[cidade_mutante];
		
							rotas_perturbadas[rota_perturbada][index_perturbado] = cidade_mutante;
							posicoes_perturbadas_rotas[cidade_mutante] = rota_perturbada;
							posicoes_perturbadas_cidades[cidade_mutante] = index_perturbado;
							cidades_fechadas[cidade_mutante] = 1;
							
							rotas_perturbadas[rota_mutante][index_mutante] = cidade_perturbada;
							posicoes_perturbadas_rotas[cidade_perturbada] = rota_mutante;
							posicoes_perturbadas_cidades[cidade_perturbada] = index_mutante;
							cidades_fechadas[cidade_perturbada] = 1;
						}
					}
				}
				
			} else if(!crossover_binario) {
				goto FINALIZA_CROSSOVER; 
			}
		}
	}

FINALIZA_CROSSOVER:
	return trial;
}

void differential_evolution_1(int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max, int best_solution, int mutation_rand, int crossover_bin) {
	Generation *generation = generation_inicial(distances, customers, customers_num, vehicles_num, capacity_max),
	        *generation2 = NULL;
	
	FILE *file_solucao = NULL,
	     *relatorio = NULL;
	
	if(IMPRESSAO_ARQUIVO) {
		file_solucao = fopen("solucao.vrp", "w");
		relatorio = fopen("relatorio.ed", "w");
		imprime_relatorio_arquivo(relatorio, generation);
		fprintf(relatorio, "\n");
	} else {
		imprime_relatorio_terminal(generation);
		printf("\n");
	}
	
	int qtd_geracoes = 1,
	    encontrou_best_solution = 0,
	    melhor_fitness = generation->best_solution->cost;
	do {
		generation2 = nova_generation_1(generation, distances, customers, customers_num, vehicles_num, capacity_max, mutacao_rand, crossover_binario);
		
		if(melhor_fitness != generation2->best_solution->cost){
			if(IMPRESSAO_ARQUIVO) {
				imprime_relatorio_arquivo(relatorio, generation2);
				fprintf(relatorio, "\n");
			} else {
				imprime_relatorio_terminal(generation2);
				printf("\n");
			}
			
			melhor_fitness = generation2->best_solution->cost;
		}
		
		generation = generation_libera(generation, vehicles_num);
		generation = generation2;
		qtd_geracoes++;
		
		generation_zera_clones(generation);
		
		if(generation->best_solution != NULL && generation->best_solution->cost == best_solution) {
			encontrou_best_solution = 1;
		}
		
	}while (!encontrou_best_solution && qtd_geracoes < NUM_MAX_GERACOES);
	

	if(IMPRESSAO_ARQUIVO) {
		if(qtd_geracoes == NUM_MAX_GERACOES){
			imprime_relatorio_arquivo(relatorio, generation2);
			fprintf(relatorio, "\n");
		}
	
		if(generation->best_solution != NULL) {
			imprime_individual_arquivo(file_solucao, generation->best_solution, vehicles_num, customers_num);
		} else  fprintf(file_solucao, "\nNão houve solução viável.\n");
		
		printf("    Verifique o arquivo solucao.vrp no disco.\n");
		fclose(relatorio);
		fclose(file_solucao);
		
	} else {
		if(qtd_geracoes == NUM_MAX_GERACOES){
			imprime_relatorio_terminal(generation2);
			printf("\n");
		}
	
		if(generation->best_solution != NULL) {
			imprime_individual_terminal(generation->best_solution, vehicles_num, customers_num);
		} else  printf("\nNão houve solução viável.\n");
	}

	generation_zera_clones(generation);
	if(generation2->best_solution->cloned){
		individual_libera(generation2->best_solution, vehicles_num);
	}
	generation = generation_libera(generation, vehicles_num);
	return;
}

void differential_evolution_2(Customer* customers, int customers_num, int vehicles_num, int capacity_max, int best_solution, int mutation_rand, int crossover_bin) {
	return;
}

void differential_evolution_2_rand_to_best(Customer* customers, int customers_num, int vehicles_num, int capacity_max, int best_solution, int crossover_bin) {
	return;
}

/* Rodando
int main(int argc, char *argv[]) {
	F = atof(argv[1]);
	CR = atof(argv[2]);
	int semente = atoi(argv[4]);

	int customers_num = 0,
	    vehicles_num = 0,
	    best_solution = 0,
	    capacity_max = 0,
	  //  escolha = escolha_tecnica(),
	    escolha = 2,
	    crossover_binario = escolha % 2; //Se a escolha é impar o crossover é binario.

	if(!crossover_binario) escolha--;
	
	if(escolha == 2) {
		printf("Técnica rand/1/exp\n");
	} else  printf("Técnica divergente!\n");
	
	char nome[] = "a";

// 	printf("    Escreva o nome do arquivo com os dados das customers.\n    Nome: ");
//	scanf("%s", nome);
	printf("\n");

	FILE* leitura = fopen(argv[3], "r");
	if(leitura == NULL) {
		printf("    O arquivo %s não foi encontrado no disco.\n    Abortando...\n\n", nome);
		return 0;
	}
	
	fscanf(leitura, "%d %d %d %d", &vehicles_num, &best_solution, &customers_num, &capacity_max);
	
	NP = 2 * customers_num;
	
	Cidade customers[customers_num];
	arquivo_inicializa_cidades(customers, leitura, customers_num);
	
	int** distances = matriz_custos_inicializa(customers, customers_num);

	fclose(leitura);
	
	printf("Semente: %d\n", semente); 
	srand( semente );
	
	switch(escolha) {//1 indica que o tipo de mutação é randomica (rand) e 0 utiliza o melhor individual (best).
		case 1 :
			EvolucaoDiferencial_1(distances, customers, customers_num, vehicles_num, capacity_max, best_solution, 1, crossover_binario); 
			break;
		case 3:
			EvolucaoDiferencial_1(distances, customers, customers_num, vehicles_num, capacity_max, best_solution, 0, crossover_binario);
			break;
		case 5:
			EvolucaoDiferencial_2(customers, customers_num, vehicles_num, capacity_max, best_solution, 1, crossover_binario);
			break;
		case 7:
			EvolucaoDiferencial_2(customers, customers_num, vehicles_num, capacity_max, best_solution, 0, crossover_binario);
			break;
		case 9:
			EvolucaoDiferencial_2_rand_to_best(customers, customers_num, vehicles_num, capacity_max, best_solution, crossover_binario);
			break;
	}
	
	distances = matriz_custos_libera(distances, customers_num);
	return 0;
}
*/

int main() {
	//F = 0.1;
	//CR = 0.9;
	int semente = time(NULL);
	
	printf("ED Combinatória!\n\n");

	int customers_num = 0,
	    vehicles_num = 0,
	    best_solution = 0,
	    capacity_max = 0,
	    escolha = escolha_tecnica(),
	    crossover_binario = escolha % 2; //Se a escolha é impar o crossover é binario.
	    
	switch(escolha){
		case 1:
			printf("Técnica rand/1/bin\n");
			break;

		case 2:
			printf("Técnica rand/1/exp\n");
			break;

		case 3:
			printf("Técnica best/1/bin\n");
			break;
		
		case 4:
			printf("Técnica best/1/exp\n");
			break;
	}

	if(!crossover_binario) escolha--;
	
	printf("F=%.2lf, CR=%.2lf, PENA=%d\n", F, CR, PENALIDADE);
	
	char nome[81];

 	printf("    Escreva o nome do arquivo com os dados das customers.\n    Nome: ");
	scanf("%s", nome);

	FILE* leitura = fopen(nome, "r");
	if(leitura == NULL) {
		printf("    O arquivo %s não foi encontrado no disco.\n    Abortando...\n\n", nome);
		return 0;
	}
	
	fscanf(leitura, "%d %d %d %d", &vehicles_num, &best_solution, &customers_num, &capacity_max);
	
	//NP = 3 * customers_num;
	printf("NP=%d\n", NP);
	
	Cidade customers[customers_num];
	arquivo_inicializa_cidades(customers, leitura, customers_num);
	
	int** distances = matriz_custos_inicializa(customers, customers_num);

	fclose(leitura);
	
	printf("Semente: %d\n\n", semente); 
	srand( semente );
	
	switch(escolha) {//1 indica que o tipo de mutação é randomica (rand) e 0 utiliza o melhor individual (best).
		case 1 :
			EvolucaoDiferencial_1(distances, customers, customers_num, vehicles_num, capacity_max, best_solution, 1, crossover_binario); 
			break;
		case 3:
			EvolucaoDiferencial_1(distances, customers, customers_num, vehicles_num, capacity_max, best_solution, 0, crossover_binario);
			break;
		case 5:
			EvolucaoDiferencial_2(customers, customers_num, vehicles_num, capacity_max, best_solution, 1, crossover_binario);
			break;
		case 7:
			EvolucaoDiferencial_2(customers, customers_num, vehicles_num, capacity_max, best_solution, 0, crossover_binario);
			break;
		case 9:
			EvolucaoDiferencial_2_rand_to_best(customers, customers_num, vehicles_num, capacity_max, best_solution, crossover_binario);
			break;
	}
	
	distances = matriz_custos_libera(distances, customers_num);
	return 0;
}






