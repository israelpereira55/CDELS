#ifndef EVOLUCAO_DIFERENCIAL_H_
#define EVOLUCAO_DIFERENCIAL_H_

#include "../Base/dependencias.h"

#define F 0.1
#define CR 0.9
/* #define NP 250 */
#define NUM_MAX_GERACOES 50000
#define PENALIDADE 100
#define IMPRESSAO_ARQUIVO 0


typedef struct geracao Geracao;

struct geracao{
	int id;
	Individuo** individuos;
	Individuo* melhor_solucao;
	int num_solucoes_viaveis;
};

Geracao* geracao_inicializa();
			
Geracao* geracao_inicial(int** distancias, Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max);

Geracao* nova_geracao_1(Geracao* geracao, int** distancias, Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max, int mutacao_rand, int crossover_binario);

void geracao_atualiza_melhores(Geracao* geracao);

Geracao* geracao_libera(Geracao* geracao, int num_veiculos);


void EvolucaoDiferencial_1(int** distancias, Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max, int melhor_solucao, int rand, int crossover_binario);

void EvolucaoDiferencial_2(Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max, int melhor_solucao, int rand, int crossover_binario);

void EvolucaoDiferencial_2_rand_to_best(Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max, int melhor_solucao, int crossover_binario);


Individuo* individuo_gera_mutante(Individuo* x1, Individuo* x2, Individuo* x3, int num_veiculos, int num_cidades);

Individuo* mutacao(Geracao* geracao, int alvo, Cidade* cidades, int num_cidades, int num_veiculos, int tipo_mutacao);

Individuo* crossover(Individuo* x1, Individuo* mutante, int crossover_binario, int num_cidades, int num_veiculos);


#endif /*EVOLUCAO_DIFERENCIAL_H_*/
