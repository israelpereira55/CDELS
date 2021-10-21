#ifndef DEPENDENCIAS_H_
#define DEPENDENCIAS_H_

typedef struct cidade Cidade;

struct cidade {
	int id, //Identificação: número da cidade.
	    demanda;
	double x,y; //Posição da cidade no espaço.
};

typedef struct individuo Individuo;

struct individuo {
	int **rotas,
	    **posicoes, // Dado um indice i no vetor posicao, o conteúdo é a posicao da cidade i no vetor percurso. Ou seja, ele indica a posicao das cidades no vetor percurso.
	     *fim_rotas,
	     *cargas_disponiveis;
	int custo,
	    viavel,
	    clonado;
};


Cidade* cidade_cria(int id, double x, double y);

//int cidade_calcula_distancia(Cidade* c1, Cidade* c2);

Cidade* cidade_libera(Cidade* cidade);

int** matriz_custos_inicializa(Cidade* cidades, int num_cidades);

int** matriz_custos_libera(int** distancias, int num_cidades);


Individuo* individuo_inicializa(int num_cidades, int num_veiculos);

Individuo* individuo_gera_mais_esquerda(int** distancias, Cidade* cidades, int num_cidades, int capacidade_max, int num_rotas);

Individuo* individuo_gera_mais_direita(int** distancias, Cidade* cidades, int num_cidades, int capacidade_max, int num_rotas);

void individuo_troca_cidades(Individuo* individuo, int cidade1, int carga1, int cidade2, int carga2);

void individuo_reinsere_cidade_rota(Individuo* individuo, int cidade, int novo_index);

void individuo_insere_cidade(Individuo* individuo, int cidade, int carga, int nova_posicao, int nova_rota);

/* Sem uso */
// void individuo_insere_cidade_fim_rota(Individuo* individuo, int cidade, int carga, int rota);

void individuo_remove_cidade(Individuo* individuo, int cidade, int carga);

void individuo_atualiza_atributos(Individuo* individuo, int capacidade_max, int num_veiculos, int** distancias, Cidade* cidades);

Individuo* individuo_clona(Individuo* individuo, int num_cidades, int num_veiculos);

Individuo* individuo_libera(Individuo* individuo, int num_veiculos);


#endif /*DEPENDENCIAS_H_*/
