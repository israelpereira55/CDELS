#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include "dependencias.h"
#include "BuscaLocal.h"

#include "../HeadED/EvolucaoDiferencial.h"

/* Determina a cidade_anterior e a cidade_posterior da cidade determinada pela componente.
 *
 * Variáveis:
 *   - (int)  componente, fim_rota, cidade_anterior, cidade_posterior.
 *   - (int*) rota.
 */
#define determina_cidade_ant_e_pos(componente, rota, fim_rota, cidade_anterior, cidade_posterior) \
	if(componente == 0){ \
		cidade_anterior = 0; \
		cidade_posterior = rota[1]; \
	} else if(componente == fim_rota -1){ \
		cidade_anterior = rota[componente -1]; \
		cidade_posterior = 0; \
	} else{ \
		cidade_anterior = rota[componente -1]; \
		cidade_posterior = rota[componente +1]; \
	}


/* Determina a cidade_anterior e a cidade_posterior da cidade determinada pela componente.
 * Mesmo código da função superior, com a diferença que se a componente estiver na ultima posição da rota,
 * o fim da rota é decrementado. (Usado quando é preciso remover a cidade de uma rota para outra).
 *
 * Variáveis:
 *   - (int)  componente, fim_rota, cidade_anterior, cidade_posterior.
 *   - (int*) rota.
 */ 
#define determina_cidade_ant_e_pos_dec_fim(componente, rota, fim_rota, cidade_anterior, cidade_posterior) \
	if(componente == 0){ \
		cidade_anterior = 0; \
		cidade_posterior = rota[1]; \
	} else if(componente == fim_rota -1){ \
		cidade_anterior = rota[componente -1]; \
		cidade_posterior = 0; \
		fim_rota--; \
	} else{ \
		cidade_anterior = rota[componente -1]; \
		cidade_posterior = rota[componente +1]; \
	}


/* Calcula o custo do indivíduo com a troca da cidade_antiga pela nova_cidade.
 *
 * Variáveis:
 *   - (int)   custo, cidade_anterior, cidade_posterior, cidade_antiga, nova_cidade.
 *   - (int**) distancias.
 */
#define calcula_custo_troca(custo, distancias, cidade_anterior, cidade_posterior, cidade_antiga, nova_cidade) \
	custo   - distancias[cidade_anterior][cidade_antiga]  \
		- distancias[cidade_antiga][cidade_posterior] \
		+ distancias[cidade_anterior][nova_cidade]    \
		+ distancias[nova_cidade][cidade_posterior]


/* Calcula o custo do indivíduo com a remoção da cidade.
 * Não deve ser usado caso a rota esteja vazia, pois é adicionado o custo da cidade anterior a cidade posterior,
 * mas mesmo que neste caso o valor ainda será correto, será menos eficiente.
 *
 * Variáveis:
 *   - (int)   custo, cidade_anterior, cidade_posterior, cidade.
 *   - (int**) distancias.
 */ 
#define calcula_remocao_cidade(custo, distancias, cidade_anterior, cidade_posterior, cidade) \
	custo   - distancias[cidade_anterior][cidade] \
		- distancias[cidade][cidade_posterior] \
		+ distancias[cidade_anterior][cidade_posterior]


/* Calcula o custo do indivíduo com adição da cidade.
 * Não deve ser usado caso a rota esteja vazia, pois é removido o custo da cidade anterior a cidade posterior.
 *
 * Variáveis:
 *   - (int)   custo, cidade_anterior, cidade_posterior, cidade.
 *   - (int**) distancias.
 */
#define calcula_adicao_cidade(custo, distancias, cidade_anterior, cidade_posterior, cidade) \
	custo   + distancias[cidade_anterior][cidade] \
		+ distancias[cidade][cidade_posterior] \
		- distancias[cidade_anterior][cidade_posterior]


/* Calcula o custo do indivíduo com a troca da cidadei pela cidadej.
 */
#define calcula_custo_com_troca_2p(rotas, distancias, novo_custo, custo, i, j, cidadei, rotai, icidade_anterior, icidade_posterior, cidadej, rotaj, jcidade_anterior, jcidade_posterior) \
	if(i == j-1){ \
	novo_custo = custo - distancias[icidade_anterior][cidadei] \
		- distancias[cidadej][jcidade_posterior] \
		+ distancias[cidadej][icidade_anterior] \
		+ distancias[cidadei][jcidade_posterior]; \
	} else { \
	novo_custo = custo + calcula_custo_troca(0, distancias, icidade_anterior, icidade_posterior, cidadei, cidadej) \
		+  calcula_custo_troca(0, distancias, jcidade_anterior, jcidade_posterior, cidadej, cidadei); \
	}

#include <stdio.h>
void Busca_Local(Individuo* perturbado, int** distancias, Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max){
	int mov_sem_melhora = 0;
	int custo_original = perturbado->custo;
	
	do{
 		two_opt(perturbado, distancias, cidades, num_cidades, num_veiculos, capacidade_max);
 		if(custo_original > perturbado->custo){
 			custo_original = perturbado->custo;
 			mov_sem_melhora = 0;
 		} else  mov_sem_melhora++;
 		
 		if(mov_sem_melhora > 1) break;
 		
 		strong_drop_one_point_viavel(perturbado, distancias, cidades, num_veiculos, num_cidades, capacidade_max);
 		if(custo_original > perturbado->custo){
 			custo_original = perturbado->custo;
 			mov_sem_melhora = 0;
 		} else  mov_sem_melhora++;
 		
 		if(!perturbado->viavel){
 			drop_one_point_inviavel(perturbado, distancias, cidades, num_veiculos, num_cidades, capacidade_max);
 			if(perturbado->viavel)
 				mov_sem_melhora = 0;
 		}
 	}while(mov_sem_melhora < 2);
 	
 	return;
 	
		
//	swap_two_points(perturbado, distancias, cidades, num_cidades, num_veiculos, capacidade_max);
	
	two_opt(perturbado, distancias, cidades, num_cidades, num_veiculos, capacidade_max);
	
//	individuo_atualiza_atributos(perturbado, capacidade_max, num_veiculos, distancias, cidades);
//	two_opt(perturbado, distancias, cidades, num_cidades, num_veiculos, capacidade_max);
	int custo = perturbado->custo;
	individuo_atualiza_atributos(perturbado, capacidade_max, num_veiculos, distancias, cidades);
	if(perturbado->custo != custo) 
		printf("%d %d, Deu ruim creuza\n", custo, perturbado->custo);
//	printf("nao\n");
//	exit(0);
	
	
	return;
	/*	if(perturbado->viavel){
		drop_one_point_viavel(perturbado, distancias, cidades, num_veiculos, num_cidades, capacidade_max);
		} else {
		drop_one_point_inviavel(perturbado, distancias, cidades, num_veiculos, num_cidades, capacidade_max);
		if(perturbado->viavel)
		drop_one_point_viavel(perturbado, distancias, cidades, num_veiculos, num_cidades, capacidade_max);
		}
	 */	
	int it_sem_melhora = 0, custo_inicial;
	while(it_sem_melhora < IT_SEM_MELHORA_DOP){
		if(perturbado->viavel){
			it_sem_melhora = 0;
			while(it_sem_melhora < IT_SEM_MELHORA_DOP){
				custo_inicial = perturbado->custo;
				drop_one_point_viavel(perturbado, distancias, cidades, num_veiculos, num_cidades, capacidade_max);

				if(custo_inicial > perturbado->custo){
					it_sem_melhora = 0;
				} else  it_sem_melhora++;
			}

		} else drop_one_point_inviavel(perturbado, distancias, cidades, num_veiculos, num_cidades, capacidade_max), it_sem_melhora++;
	}

	flip(perturbado, distancias, cidades, num_veiculos, num_cidades);

	return;
}

void two_opt(Individuo* individuo, int** distancias, Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max){
	int i, j,
	    rotai, rotaj,
	    carga, cargaj,
	    cidadei, cidadej,
	    fim_rotai, fim_rotaj,
	    icidade_anterior, icidade_posterior,
	    jcidade_anterior, jcidade_posterior;
	 
	int** rotas = individuo->rotas;
	
	int *rota,
	    *fim_rotas = individuo->fim_rotas,
	    *cargas_disponiveis = individuo->cargas_disponiveis;
	
	int novo_custo,
	    custo_original = individuo->custo;
	 
//	    houve_melhora_mov,
	int houve_melhora_it;
	    
 //	do{
//		houve_melhora_it = 0;
//		houve_melhora_mov = 0;
		for (rotai = 0; rotai < num_veiculos; rotai++) {
		
			fim_rotai = fim_rotas[rotai];
			for(i = 0; i < fim_rotai; i++){
REINICIA_MOV_2OPT:
				cidadei = rotas[rotai][i];
				carga = cidades[cidadei].demanda;
				
				for(rotaj = rotai; rotaj < num_veiculos; rotaj++){

					fim_rotaj = fim_rotas[rotaj];
					if(rotai == rotaj) {
						j = i+1;
					} else  j = 0;
					
					for (; j < fim_rotaj; j++) {
						/*Verificando se a troca é viável. Caso os dois pontos estejam na mesma rota o movimento não inviabilizará o indivíduo. */
						
						cidadej = rotas[rotaj][j];
						cargaj = cidades[cidadej].demanda;
				
						if(rotai != rotaj){
							if((cargas_disponiveis[rotai] + carga < cargaj) || (cargas_disponiveis[rotaj] + cargaj < carga)){
								continue;
							} else {
								rota = rotas[rotai];
								rota[fim_rotai] = 0;
								determina_cidade_ant_e_pos(i, rota, fim_rotai, icidade_anterior, icidade_posterior); /* Macro */
							
								rota = rotas[rotaj];
								rota[fim_rotaj] = 0;
								determina_cidade_ant_e_pos(j, rota, fim_rotaj, jcidade_anterior, jcidade_posterior);
							
								novo_custo = custo_original + calcula_custo_troca(0, distancias, icidade_anterior, icidade_posterior, cidadei, cidadej)
											    + calcula_custo_troca(0, distancias, jcidade_anterior, jcidade_posterior, cidadej, cidadei);
								
								if(novo_custo < custo_original){
									custo_original = novo_custo;
									individuo_troca_cidades(individuo, cidadei, carga, cidadej, cargaj);
									
									houve_melhora_it = 1; 
//									if(!houve_melhora_mov) houve_melhora_mov = 1;
								}
							}
						} else {
							rota = rotas[rotai];
							determina_cidade_ant_e_pos(i, rota, fim_rotai, icidade_anterior, icidade_posterior); /* Macro */
						
							rota = rotas[rotaj];
							determina_cidade_ant_e_pos(j, rota, fim_rotaj, jcidade_anterior, jcidade_posterior); /* Macro */

							calcula_custo_com_troca_2p(rotas, distancias, novo_custo, custo_original, i, j, cidadei, rotai, icidade_anterior, icidade_posterior, cidadej, rotaj, jcidade_anterior, jcidade_posterior);

							if(novo_custo < custo_original){
								custo_original = novo_custo;
								individuo_troca_cidades(individuo, cidadei, 0, cidadej, 0); /* Como estão na mesma rota, as cargas não mudarão */
					
								houve_melhora_it = 1;
//								if(!houve_melhora_mov) houve_melhora_mov = 1;
							}
						}
						
						if(houve_melhora_it){
							houve_melhora_it = 0;
							goto REINICIA_MOV_2OPT;
						}
					}
				}
			}
		}
//	}while(houve_melhora_mov);

	if(!individuo->viavel){
		i = 0;
		while(i < num_veiculos && cargas_disponiveis[i] >=0) i++;

		if(i == num_veiculos){
			individuo->custo = custo_original - PENALIDADE;
			individuo->viavel = 1;
			return;
		}
	}
	
	individuo->custo = custo_original;
	return;
}

void swap_two_points(Individuo* individuo, int** distancias, Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max){
	int i,
	    carga,
	    fim_rota;

	int cargas_disponiveis[num_veiculos];

	int **rotas = individuo->rotas,
	    *rota,
	    *fim_rotas = individuo->fim_rotas,
	    *posicoes_rota = individuo->posicoes[0],
	    *posicoes_cidades = individuo->posicoes[1];

	int rota_original = 0;
	do{
		rota = rotas[rota_original];
		fim_rota = fim_rotas[rota_original];

		carga = 0;
		for(i = 0; i < fim_rota; i++)
			carga = carga + cidades[ rota[i] ].demanda;

		cargas_disponiveis[rota_original] = capacidade_max - carga;
		rota_original++;
	} while(rota_original < num_veiculos);

	int cidade,
	    nova_rota,
	    novo_custo,
	    nova_carga,
	    componente,
	    nova_cidade,
	    nova_componente,
	    cidade_anterior,
	    cidade_posterior;

	int houve_melhora,
	    it_sem_melhora = 0,
	    custo_original = individuo->custo;
	do{
		cidade = 1;
		houve_melhora = 0;
		do{
			rota_original = posicoes_rota[cidade];
			componente = posicoes_cidades[cidade];

			/* Obtendo a nova cidade aleatória. */
			do{
				nova_rota = rand() % num_veiculos;
			}while(nova_rota == rota_original || fim_rotas[nova_rota] == 0);

			nova_componente = rand() % fim_rotas[nova_rota];
			nova_cidade = rotas[nova_rota][nova_componente];
			nova_carga = cidades[nova_cidade].demanda;
			carga = cidades[cidade].demanda;

			/* Verifica se a troca é viável */
			if(cargas_disponiveis[nova_rota] + nova_carga >= carga && cargas_disponiveis[rota_original] + carga >= nova_carga){
				fim_rota = fim_rotas[rota_original];

				/* Calculando o custo da nova cidade na posicao da cidade antiga. */
				if(fim_rota == 1){
					novo_custo = calcula_custo_troca(custo_original, distancias, 0, 0, cidade, nova_cidade); /* Macro */
				} else {
					rota = rotas[rota_original];
					determina_cidade_ant_e_pos(componente, rota, fim_rota, cidade_anterior, cidade_posterior);
					novo_custo = calcula_custo_troca(custo_original, distancias, cidade_anterior, cidade_posterior, cidade, nova_cidade); /* Macro */
				}

				/* Calculando o custo da cidade antiga na posicao da nova cidade. */
				fim_rota = fim_rotas[nova_rota];
				rota = rotas[nova_rota];
				if(fim_rota == 1){
					novo_custo = calcula_custo_troca(novo_custo, distancias, 0, 0, nova_cidade, cidade); /* Macro */
				} else {
					rota[fim_rota] = 0; /* Apenas para modularidade, veja que é utilizado rota[1] para determinação. Se a rota estiver vazia o caso se torna o mesmo caso não esteja.*/

					determina_cidade_ant_e_pos(nova_componente, rota, fim_rota, cidade_anterior, cidade_posterior); /* Macro */
					novo_custo = calcula_custo_troca(novo_custo, distancias, cidade_anterior, cidade_posterior, nova_cidade, cidade); /* Macro */
				}

				if(novo_custo < custo_original){
					individuo_troca_cidades(individuo, cidade, carga, nova_cidade, nova_carga);
					custo_original = novo_custo;
					cargas_disponiveis[nova_rota] = cargas_disponiveis[nova_rota] + nova_carga - carga;
					cargas_disponiveis[rota_original] = cargas_disponiveis[rota_original] + carga - nova_carga;

					houve_melhora = 1;
				}
			}

			cidade++;
		}while(cidade < num_cidades);

		if(houve_melhora){
			it_sem_melhora = 0;
		} else  it_sem_melhora++;

	}while(it_sem_melhora < IT_SEM_MELHORA_STP);

	if(!individuo->viavel){
		i = 0;
		while(i < num_veiculos && cargas_disponiveis[i] >=0) i++;

		if(i == num_veiculos){
			individuo->custo = custo_original - PENALIDADE;
			individuo->viavel = 1;
			return;
		}
	}

	individuo->custo = custo_original;
	return;
}

/*
 * Retorno: 0 se todas as cidades possíveis não melhoraram a solucao
 * -1 se n obteve cidade possivel
 * o novo valor do individuo caso obteve solucao
 */
int drop_one_point_inviavel(Individuo* individuo, int** distancias, Cidade* cidades, int num_veiculos, int num_cidades, int capacidade_max){	
	int* cargas_disponiveis = individuo->cargas_disponiveis;
	int inviavel = 1;

	/* Obtendo a rota de maior carga */
	int i,
	    rota_maior_carga = 0,
	    maior_carga_disponivel = 0;
	for(i = 0; i < num_veiculos; i++){
		if(cargas_disponiveis[i] > maior_carga_disponivel){
			maior_carga_disponivel = cargas_disponiveis[i];
			rota_maior_carga = i;
		}
	}

	/* Identificando a rota que estourou a capacidade. */
	int rota_invalida = 0;
	while(cargas_disponiveis[rota_invalida] >= 0) rota_invalida++;

	int** rotas = individuo->rotas;
	int *rota = NULL,
	    *fim_rotas = individuo->fim_rotas;
	
	int carga,
	    cidade,
	    fim_rota,
	    rota_selecionada,
	    tentativas,
	    max_tentativas;
	do{	
		/* Selecionando uma rota inválida aleatória */
		do{
			rota_invalida = rand() % num_veiculos;
		}while(cargas_disponiveis[rota_invalida] >= 0);
		
		rota = rotas[rota_invalida];
		fim_rota = fim_rotas[rota_invalida];
		
		/* Tentativa de selecionar uma cidade aleatória na rota que estourou a capacidade */
		tentativas = 0;
		max_tentativas = fim_rota + fim_rota/2;
		do{
			if(tentativas == max_tentativas) 
				return -1; /* Algoritmo chegou ao máximo de tentativas e não encontrou uma cidade que pudesse ser realocada naquela rota */
		
			cidade = rota[rand() % fim_rota];
			carga = cidades[cidade].demanda;
			
			tentativas++;
		}while(maior_carga_disponivel < carga);
		
		
		/* Selecionando uma rota aleatória para realocar a cidade selecionada */
		do{
			rota_selecionada = rand() % num_veiculos;
		}while(cargas_disponiveis[rota_selecionada] < carga);

		reinsere_cidade_melhor_posicao_outra_rota(individuo, distancias, cidade, carga, rota_selecionada);

//		cargas_disponiveis[num_rota] -= carga;
//		cargas_disponiveis[rota_invalida] += carga;

		//Se a rota selecionada foi a rota de maior carga disponivel, a maior carga disponivel será recalculada.
		if(rota_selecionada == rota_maior_carga){
			maior_carga_disponivel -= carga;
			for(i = 0; i < num_veiculos; i++){
				if(maior_carga_disponivel < cargas_disponiveis[i]){
					maior_carga_disponivel = cargas_disponiveis[i];
					rota_maior_carga = i;
				}
			}
		}
		
		/* Será verificado se o individuo viabilizou */
		if(cargas_disponiveis[rota_invalida] >= 0){
			for(i = 0; i < num_veiculos; i++){
				if(cargas_disponiveis[i] < 0){
					break;
				}
			}
			
			if(i == num_veiculos) inviavel = 0;
		}
		
	}while(inviavel);

	individuo->viavel = 1;
	return individuo->custo = individuo->custo - PENALIDADE;
}

/*
 * Retorno: 0 se todas as cidades possíveis não melhoraram a solucao
 * -1 se n obteve cidade possivel
 * o novo valor do individuo caso obteve solucao
 */
void drop_one_point_viavel(Individuo* individuo, int** distancias, Cidade* cidades, int num_veiculos, int num_cidades, int capacidade_max){
	int i,
	    fim_rota,
	    carga,
	    capacidade_disponivel,
	    rota_maior_carga = 0;

	int **rotas = individuo->rotas,
	    *rota = NULL,
	    *fim_rotas = individuo->fim_rotas;

	int cargas_disponiveis[num_veiculos],
	    cidades_possiveis[num_cidades],
	    rotas_selecionadas[num_veiculos];

	int num_rota = 0,
	    maior_carga_disponivel = 0;

	//Calculando a capacidade disponível de cada rota.
	do {
		fim_rota = fim_rotas[num_rota];
		rota = rotas[num_rota];

		carga = 0;
		for(i = 0; i < fim_rota; i++){
			carga = carga + cidades[ rota[i] ].demanda;
		}

		capacidade_disponivel = capacidade_max - carga;
		if(capacidade_disponivel > maior_carga_disponivel){
			maior_carga_disponivel = capacidade_disponivel;
			rota_maior_carga = num_rota;
		}

		cargas_disponiveis[num_rota] = capacidade_disponivel;
		num_rota++;
	} while(num_rota < num_veiculos);

	int segunda_maior_carga_disponivel = 0,
	    segunda_rota_maior_carga = 0,
	    primeira_melhor = 1;

	/* Calculando a primeira e segunda rota de maior carga. */
	for(i = 0; i < num_veiculos; i++){
		carga = cargas_disponiveis[i];
		if(carga > segunda_maior_carga_disponivel){
			if(carga != maior_carga_disponivel){
				segunda_maior_carga_disponivel = carga;
				segunda_rota_maior_carga = i;
			} else if(primeira_melhor){
				primeira_melhor = 0;
			} else {
				segunda_maior_carga_disponivel = carga;
				segunda_rota_maior_carga = i;
			}
		}
	}

	int cidade = 0,
	    max_index_cidades = 0;

	for(num_rota = 0; num_rota < num_veiculos; num_rota++){
		fim_rota = fim_rotas[num_rota];
		rota = rotas[num_rota];

		if(num_rota != rota_maior_carga){
			for(i = 0; i < fim_rota; i++){
				cidade = rota[i];	
				if(maior_carga_disponivel >= cidades[cidade].demanda){
					cidades_possiveis[max_index_cidades] = cidade;
					max_index_cidades++;
				}
			}
		} else {
			for(i = 0; i < fim_rota; i++){
				cidade = rota[i];	
				if(segunda_maior_carga_disponivel >= cidades[cidade].demanda){
					cidades_possiveis[max_index_cidades] = cidade;
					max_index_cidades++;
				}
			}
		}
	}

	if(max_index_cidades == 0) return;

	int* posicoes_rotas = individuo->posicoes[0];

	int index,
	    obteve_melhora,
	    max_index_rotas,
	    rota_selecionada,
	    cidade_perturbada,
	    perturbacoes = 0,
	    num_perturbacoes = 0.2 * num_cidades;

	int cidades_fechadas[num_cidades];
	memset(cidades_fechadas, 0, num_cidades*sizeof(int));

	do{	
		i = 0;
		do{
			cidade_perturbada = cidades_possiveis[rand() % max_index_cidades];
			i++;
		} while(cidades_fechadas[cidade_perturbada] && i < max_index_cidades);

		if(i == max_index_cidades) return;

		cidades_fechadas[cidade_perturbada] = 1;
		num_rota = posicoes_rotas[cidade_perturbada];
		carga = cidades[cidade_perturbada].demanda;

		max_index_rotas = 0;
		for(i = 0; i < num_veiculos; i++){
			if(cargas_disponiveis[i] >= carga && i != num_rota){
				rotas_selecionadas[max_index_rotas] = i;
				max_index_rotas++;
			}
		}

		if(max_index_rotas == 0) return;

		num_rota = rotas_selecionadas[rand() % max_index_rotas]; //nova rota.
		rota_selecionada = posicoes_rotas[cidade_perturbada];
		obteve_melhora = reinsere_cidade_melhor_posicao_outra_rota_caso_melhore(individuo, distancias, cidade_perturbada, carga, num_rota);

		if(obteve_melhora){
			cargas_disponiveis[num_rota] -= carga;
			cargas_disponiveis[rota_selecionada] += carga;

			/* Se a rota selecionada foi a rota de maior carga disponivel, a maior carga disponivel será recalculada. */
			if(num_rota == rota_maior_carga){
				maior_carga_disponivel -= carga;
				for(i = 0; i < num_veiculos; i++){
					if(maior_carga_disponivel < cargas_disponiveis[i]){
						maior_carga_disponivel = cargas_disponiveis[i];
						rota_maior_carga = i;
					}
				}
			}
			//A segunda maior carga disponivel também será recalculada caso seja necessário.
			if(num_rota == segunda_rota_maior_carga || segunda_rota_maior_carga >= maior_carga_disponivel){
				primeira_melhor = 1;
				for(i = 0; i < num_veiculos; i++){
					carga = cargas_disponiveis[i];
					if(carga > segunda_maior_carga_disponivel){
						if(carga != maior_carga_disponivel){
							segunda_maior_carga_disponivel = carga;
							segunda_rota_maior_carga = i;
						} else if(primeira_melhor){
							primeira_melhor = 0;
						} else {
							segunda_maior_carga_disponivel = carga;
							segunda_rota_maior_carga = i;
						}
					}
				}
			}

			/* Removendo as cidades que não possuem mais alguma rota que a suporte, diferente da rota na qual a cidade reside. */
			i = 0;
			do{
				cidade = cidades_possiveis[i];
				i++;
			}while(i < max_index_cidades && cidade != cidade_perturbada && cidades[cidade].demanda <= maior_carga_disponivel);

			/* Caso encontre ao menos uma cidade a ser removida, os elementos da rota serão deslocados. */
			if(i < max_index_cidades){
				index = i -1;
				while(i < max_index_cidades){
					cidade = cidades_possiveis[i];

					num_rota = posicoes_rotas[cidade];
					if(num_rota != rota_maior_carga){
						if(cidade != cidade_perturbada && cidades[cidade].demanda <= maior_carga_disponivel){
							cidades_possiveis[index] = cidades_possiveis[i];
							index++;
						}
					} else if(cidade != cidade_perturbada && cidades[cidade].demanda <= segunda_maior_carga_disponivel){
						cidades_possiveis[index] = cidades_possiveis[i];
						index++;
					}

					i++;
				}
				max_index_cidades = index;

			} else max_index_cidades--;

			if(max_index_cidades == 0) return;
		}

		perturbacoes++;
	}while(perturbacoes < num_perturbacoes);

	return;
}

void flip(Individuo* individuo, int** distancias, Cidade* cidades, int num_veiculos, int num_cidades){
	int index,
	    num_rota,
	    fim_rota,
	    cidade,
	    componente,
	    custo_base,
	    novo_custo,
	    nova_posicao = 0,
	    cidade_anterior,
	    cidade_posterior,
	    obteve_melhora,
	    houve_melhora_mov,
	    custo_original = individuo->custo;

	int **rotas = individuo->rotas,
	    *rota = NULL,
	    *fim_rotas = individuo->fim_rotas;

	int it_sem_melhora = 0;
	do{
		num_rota = 0;
		houve_melhora_mov = 0;
		do{
			obteve_melhora = 0;
			fim_rota = fim_rotas[num_rota];
			if(fim_rota < 2){
				if(num_rota +1 < num_veiculos){
					num_rota++;
					fim_rota = fim_rotas[num_rota];
				} else goto PROX_ITERACAO_FLIP;
			}

			rota = rotas[num_rota];
			componente = rand() % fim_rota;
			cidade = rota[componente];

			determina_cidade_ant_e_pos_dec_fim(componente, rota, fim_rota, cidade_anterior, cidade_posterior); /* Macro */
			custo_base = calcula_remocao_cidade(custo_original, distancias, cidade_anterior, cidade_posterior, cidade); /* Macro */

			rota[ fim_rotas[num_rota] ] = 0;
			fim_rota++;

			index = 0;
			if(componente != 0){
				cidade_anterior = 0;
				cidade_posterior = rota[0];
			} else {
				index = 2;
				cidade_anterior = cidade_posterior;
				cidade_posterior = rota[2];
			}

			while(index < fim_rota){
				novo_custo = calcula_adicao_cidade(custo_base, distancias, cidade_anterior, cidade_posterior, cidade); /* Macro */

				if(novo_custo < custo_original){
					custo_original = novo_custo;
					nova_posicao = index;		
					obteve_melhora = houve_melhora_mov = 1;
				}

				index++;
				if(index == componente) index++;

				cidade_anterior = cidade_posterior;
				cidade_posterior = rota[index];
			}

			if(obteve_melhora){
				individuo_reinsere_cidade_rota(individuo, cidade, nova_posicao);
				individuo->custo = custo_original;
			}

			num_rota++;
		}while(num_rota < num_veiculos);

PROX_ITERACAO_FLIP:
		if(houve_melhora_mov){
			it_sem_melhora = 0;
		} else  it_sem_melhora++;

	}while(it_sem_melhora < IT_SEM_MELHORA_FLIP);

	return;
}

/* Retorno:
 *   - 0 se não houve melhora.
 *   - 1 se houve melhora.
 */
int reinsere_cidade_melhor_posicao_outra_rota_caso_melhore(Individuo* individuo, int** distancias, int cidade, int carga, int nova_rota){
	int **rotas = individuo->rotas,
	    *fim_rotas = individuo->fim_rotas;

	int cidade_anterior,
	    cidade_posterior,
	    index_rota = individuo->posicoes[0][cidade],
	    fim_rota = fim_rotas[index_rota];

	int* rota = rotas[index_rota];	
	int componente = individuo->posicoes[1][cidade];

	determina_cidade_ant_e_pos_dec_fim(componente, rota, fim_rota, cidade_anterior, cidade_posterior); /* Macro */
//	printf("%d %d %d %d\n", individuo->custo, cidade_anterior, cidade_posterior, cidade);
	int custo_base = calcula_remocao_cidade(individuo->custo, distancias, cidade_anterior, cidade_posterior, cidade); /* Macro */

	//	individuo_remove_cidade(individuo, cidade);

	int novo_custo = 0;
	if(fim_rotas[nova_rota] == 0){
		novo_custo = custo_base + distancias[0][cidade]
					+ distancias[cidade][0];

		if(novo_custo < individuo->custo){
			individuo_remove_cidade(individuo, cidade, carga);
			individuo_insere_cidade(individuo, cidade, carga, 0, nova_rota);
			individuo->custo = novo_custo;
			return 1;
		}
		return 0;
	}

	rota = rotas[nova_rota]; 
	cidade_anterior = 0;
	cidade_posterior = rota[0];
	fim_rota = fim_rotas[nova_rota];

	int index = 0,
	    custo_original = INT_MAX,
	    nova_posicao = 0;

	rota[fim_rota] = 0;
	fim_rota++;
	while(index < fim_rota){
		novo_custo = calcula_adicao_cidade(custo_base, distancias, cidade_anterior, cidade_posterior, cidade); /* Macro */

		if(novo_custo < custo_original){
			custo_original = novo_custo;
			nova_posicao = index;
		}

		index++;
		cidade_anterior = cidade_posterior;
		cidade_posterior = rota[index];
	}

	if(custo_original < individuo->custo){
		individuo_remove_cidade(individuo, cidade, carga);
		individuo_insere_cidade(individuo, cidade, carga, nova_posicao, nova_rota);
		individuo->custo = custo_original;
		return 1;
	}
	return 0;
}

void reinsere_cidade_melhor_posicao_outra_rota(Individuo* individuo, int** distancias, int cidade, int carga, int nova_rota){
	int **rotas = individuo->rotas,
	    *fim_rotas = individuo->fim_rotas;

	int cidade_anterior,
	    cidade_posterior,
	    index_rota = individuo->posicoes[0][cidade],
	    fim_rota = fim_rotas[index_rota];

	int* rota = rotas[index_rota];
	int componente = individuo->posicoes[1][cidade];

	determina_cidade_ant_e_pos_dec_fim(componente, rota, fim_rota, cidade_anterior, cidade_posterior); /* Macro */
	int custo_base = calcula_remocao_cidade(individuo->custo, distancias, cidade_anterior, cidade_posterior, cidade); /* Macro */

	individuo_remove_cidade(individuo, cidade, carga);

	if(fim_rotas[nova_rota] == 0){
		individuo_insere_cidade(individuo, cidade, carga, 0, nova_rota);
		individuo->custo = custo_base + distancias[0][cidade]
			+ distancias[cidade][0];
		return;
	}

	rota = rotas[nova_rota]; 
	cidade_anterior = 0;
	cidade_posterior = rota[0];

	int novo_custo,
	    nova_posicao = 0,
	    custo_original = INT_MAX;

	fim_rota = fim_rotas[nova_rota];
	rota[fim_rota] = 0;
	fim_rota++;

	int index = 0;
	while(index < fim_rota){
		novo_custo = calcula_adicao_cidade(custo_base, distancias, cidade_anterior, cidade_posterior, cidade); /* Macro */

		if(novo_custo < custo_original){
			custo_original = novo_custo;
			nova_posicao = index;
		}

		index++;
		cidade_anterior = cidade_posterior;
		cidade_posterior = rota[index];
	}

	individuo_insere_cidade(individuo, cidade, carga, nova_posicao, nova_rota);
	individuo->custo = custo_original;
	return;
}


void strong_drop_one_point_viavel(Individuo* individuo, int** distancias, Cidade* cidades, int num_veiculos, int num_cidades, int capacidade_max){
	int rotas_possiveis[num_veiculos];

	int i, k, l,
	    carga,
	    cidade,
	    fim_rota,
	    num_rota,
	    rota_selecionada;

	int obteve_melhora = 0,
	    carga_anterior = -1;
	
	int* rota;
	int *fim_rotas = individuo->fim_rotas,
	    *cargas_disponiveis = individuo->cargas_disponiveis;
	
	int index = 0;
	int** rotas = individuo->rotas;
	for (num_rota = 0; num_rota < num_veiculos; num_rota++) {
	
		rota = rotas[num_rota];
		fim_rota = fim_rotas[num_rota];
		carga_anterior = -1;
		
		/*Se a rota só possui uma cidade, esta cidade será mantida */
		if(fim_rota > 1){ 
			for(i = 0; i < fim_rota; i++){
				cidade = rota[i];
				carga = cidades[cidade].demanda;
				
				/* Se a carga atual é maior que a carga anterior ou houve melhora, é necessário recalcular as rotas possiveis */
				if(carga > carga_anterior || obteve_melhora){
					//Seleciona as rotas possiveis, diferentes da rota atual, para efetuar o movimento.
					index = 0;
					for(k = 0; k < num_veiculos; k++){
						if(k != num_rota && carga <= cargas_disponiveis[k]){
							rotas_possiveis[index] = k;
							index++;
						}
					}
				}
				
				obteve_melhora = 0;
				for(l = 0; l < index; l++){
					rota_selecionada = rotas_possiveis[l];
					if(!obteve_melhora){
						obteve_melhora = reinsere_cidade_melhor_posicao_outra_rota_caso_melhore(individuo, distancias, cidade, carga, rota_selecionada);
					} else  reinsere_cidade_melhor_posicao_outra_rota_caso_melhore(individuo, distancias, cidade, carga, rota_selecionada);
				}
					
				/* Com a melhora, o tamanho das rotas modificaram */
				if(obteve_melhora)
					fim_rota = fim_rotas[num_rota];
				
				carga_anterior = carga;
			}
		}
	}

	if(!individuo->viavel){
		i = 0;
		while(i < num_veiculos && cargas_disponiveis[i] >=0) i++;

		if(i == num_veiculos){
			individuo->custo -= PENALIDADE;
			individuo->viavel = 1;
		}
	}
	return;
}





