#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include "../Base/BuscaLocal.h"
#include "../Base/dependencias.h"
#include "../Base/arquivo.h"

#include "../HeadED/EvolucaoDiferencial.h"

double F, CR;
int NP;

Geracao* geracao_inicializa() {
	static int id = 1;

	Geracao* geracao = (Geracao*) malloc (sizeof(Geracao));
	geracao->individuos = (Individuo**) malloc (NP * sizeof(Individuo*));

/*	int i = 0;
	while(i < NP) {
		geracao->individuos[i] = NULL;
		i++;
	}
*/
	geracao->id = id;
	geracao->melhor_solucao = NULL;
	geracao->num_solucoes_viaveis = 0;
	id++;
	
	return geracao;
}

Geracao* geracao_inicial(int** distancias, Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max) {	
	Geracao* geracao = geracao_inicializa();
	Individuo* individuo;
	
	int seleciona = rand() % 2;
	if(seleciona == 1) {
		individuo = individuo_gera_mais_esquerda(distancias, cidades, num_cidades, capacidade_max, num_veiculos);
	} else  individuo = individuo_gera_mais_direita(distancias, cidades, num_cidades, capacidade_max, num_veiculos);
	
	geracao->individuos[0] = individuo;
	geracao->melhor_solucao = individuo;
	
	if(individuo->viavel)
		geracao->num_solucoes_viaveis++;
	
	int i = 1;
	while(i < NP) {
		seleciona = rand() % 2;
		if(seleciona == 1) {
			individuo = individuo_gera_mais_esquerda(distancias, cidades, num_cidades, capacidade_max, num_veiculos);
		} else  individuo = individuo_gera_mais_direita(distancias, cidades, num_cidades, capacidade_max, num_veiculos);
		
		geracao->individuos[i] = individuo;

		if(individuo->viavel) {
			geracao->num_solucoes_viaveis++;
			if(individuo->custo < geracao->melhor_solucao->custo)
				geracao->melhor_solucao = individuo;
		}
		i++;
	}
	
	return geracao;
}

void geracao_zera_clones(Geracao* geracao){
	int i;
	for(i = 0; i < NP; i++){
		if(geracao->individuos[i]->clonado)
			geracao->individuos[i]->clonado = 0;
	}
}

Geracao* nova_geracao_1(Geracao* geracao, int** distancias, Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max, int mutacao_rand, int crossover_binario){
	Individuo *mutante = NULL,
		  *perturbado = NULL,
		  *individuo_alvo = NULL;
	
	Geracao* geracao2 = geracao_inicializa();
	geracao2->melhor_solucao = geracao->melhor_solucao;

	if(geracao2->melhor_solucao != NULL)
		geracao2->melhor_solucao->clonado = 1;

	int geracao2_possui_melhor_solucao_geracao1 = 0;
	
	int custo_perturbado = 0;
	int alvo = 0;
	while(alvo < NP) {
		mutante = mutacao(geracao, alvo, cidades, num_cidades, num_veiculos, mutacao_rand);
		individuo_alvo = geracao->individuos[alvo];
		perturbado = crossover(individuo_alvo, mutante, crossover_binario, num_cidades, num_veiculos);
		
		mutante = individuo_libera(mutante, num_veiculos);
		individuo_atualiza_atributos(perturbado, capacidade_max, num_veiculos, distancias, cidades);

		Busca_Local(perturbado, distancias, cidades, num_cidades, num_veiculos, capacidade_max);
		
		custo_perturbado = perturbado->custo;
		if(custo_perturbado < individuo_alvo->custo){
			if(perturbado->viavel){
				geracao2->num_solucoes_viaveis++;
				if(geracao2->melhor_solucao != NULL){
					if(custo_perturbado < geracao2->melhor_solucao->custo){	
				
						if(geracao2->melhor_solucao->clonado){
							geracao2->melhor_solucao->clonado = 0;
						}
						geracao2->melhor_solucao = perturbado;
					}
				}
				
				geracao2->individuos[alvo] = perturbado;
				
			/* Se o perturbado for inviável mas tiver custo melhor que o melhor individuo, ele nao substituirá o alvo (best). */	
			} else if(individuo_alvo == geracao2->melhor_solucao){
				geracao2->individuos[alvo] = individuo_alvo;
				individuo_alvo->clonado = 1;
				geracao2->num_solucoes_viaveis++;
				geracao2_possui_melhor_solucao_geracao1 = 1;
				perturbado = individuo_libera(perturbado, num_veiculos);
				
			} else  geracao2->individuos[alvo] = perturbado;

		} else {
			geracao2->individuos[alvo] = individuo_alvo;
			individuo_alvo->clonado = 1;
			perturbado = individuo_libera(perturbado, num_veiculos);
			
			if(individuo_alvo->viavel){
				geracao2->num_solucoes_viaveis++;
				
				if(individuo_alvo == geracao2->melhor_solucao)
					geracao2_possui_melhor_solucao_geracao1 = 1;
			}
		}
		alvo++;
	}

	if(geracao2_possui_melhor_solucao_geracao1)
		geracao->melhor_solucao->clonado = 1;
	
	return geracao2;
}

Geracao* geracao_libera(Geracao* geracao, int num_veiculos) {
	int i;
	
	for(i = 0; i < NP; i++) {
		if(!geracao->individuos[i]->clonado)
			geracao->individuos[i] = individuo_libera(geracao->individuos[i], num_veiculos);
	}
	
	free(geracao->individuos);
	free(geracao);
	return NULL;
}



typedef struct cid{
	double c; //cidade continua
	int i; //index
} Cid;

int compara_cid(const void * cid1, const void * cid2){
	return (*(Cid*)cid1).c - (*(Cid*)cid2).c;
}

Individuo* individuo_gera_mutante(Individuo* x1, Individuo* x2, Individuo* x3, int num_veiculos, int num_cidades) {
	Individuo* mutante = individuo_inicializa(num_cidades, num_veiculos);
	
	int i, j,
	    fim;
	int *rota_mutante,
	    *fim_rotas_x1 = x1->fim_rotas,
	    *fim_rotas_x2 = x2->fim_rotas,
	    *fim_rotas_x3 = x3->fim_rotas;
	int *rota_x2,
	    *rota_x3;
	
	double cid2, cid3;
	int ncid = num_cidades -1;
	
	int index = 0;	
	Cid vet_cid[ncid];
	for(i = 0; i < num_veiculos; i++){
		fim = fim_rotas_x1[i];
		
		rota_x2 = x2->rotas[i];
		rota_x3 = x3->rotas[i];
		
		for(j = 0; j < fim; j++){
			cid2 = (j < fim_rotas_x2[i]) ? rota_x2[j] : (rand() % ncid) +1;
			cid3 = (j < fim_rotas_x3[i]) ? rota_x3[j] : (rand() % ncid) +1;
			
			vet_cid[index].c = (double) x1->rotas[i][j] + F*(cid2 - cid3);
			vet_cid[index].i = index;
			index++;
		}
		
		mutante->fim_rotas[i] = fim;
	}
	
/*	for(i = 0; i <= ncid; i++){
		printf("%lf -- %d\n", vet_cid[i].c, vet_cid[i].i);
	}
	printf("\n\n\n\n\n\n\n\n\nDEPOIS:\n");
*/	
	qsort(vet_cid, ncid, sizeof(Cid), compara_cid);
	
	int cid_convertidas[ncid];
	for(i = 0; i < ncid; i++)
		cid_convertidas[ vet_cid[i].i ] = i+1;
	
	int** posicoes = mutante->posicoes;
	
	int cidade;
	index = 0;
	for(i = 0; i < num_veiculos; i++){
		fim = fim_rotas_x1[i];
		rota_mutante = mutante->rotas[i];
		
		for(j = 0; j < fim; j++){
			cidade = cid_convertidas[index];
			
			rota_mutante[j] = cidade;
			posicoes[0][cidade] = i;
			posicoes[1][cidade] = j;
			index++;
		}
	}
	
	return mutante;
}

/* tipo_mutacao = 0 o individuo alvo é a melhor solucao da populacao.
 * tipo_mutacao = 1 o individuo alvo é aleatório.
 */
Individuo* mutacao(Geracao* geracao, int alvo, Cidade* cidades, int num_cidades, int num_veiculos, int tipo_mutacao){
	int r1 = rand() % NP;
	int r2 = rand() % NP;
	int r3 = rand() % NP;

	while(r2 == alvo) 
		r2 = rand() % NP;
	
	while(r3 == alvo || r3 == r2) 
		r3 = rand() % NP;

	if(tipo_mutacao == 0 && geracao->melhor_solucao != NULL)
		return individuo_gera_mutante(geracao->melhor_solucao, geracao->individuos[r2], geracao->individuos[r3], num_veiculos, num_cidades);
	
	while(r1 == alvo || r1 == r2 || r1 == r3) 
		r1 = rand() % NP;
	
	return individuo_gera_mutante(geracao->individuos[r1], geracao->individuos[r2], geracao->individuos[r3], num_veiculos, num_cidades);
}

/* A cidade perturbada é a cidade da posicao antiga do individuo, que será substituida pela cidade mutante. */
Individuo* crossover(Individuo* x1, Individuo* mutante, int crossover_binario, int num_cidades, int num_veiculos){
	int *mutante_fim_rotas = mutante->fim_rotas;
	
	int j_rand_rota = rand() % num_veiculos;
	while(mutante_fim_rotas[j_rand_rota] == 0) {
		if(j_rand_rota < num_veiculos -1) {
			j_rand_rota++; 
		} else  j_rand_rota = 0;
	}
	
	int j_rand_componente = rand() % mutante_fim_rotas[j_rand_rota];
	
	int cidades_fechadas[num_cidades];
	memset(cidades_fechadas, 0, num_cidades*sizeof(int));
	cidades_fechadas[0] = 1;
	
	Individuo* perturbado = individuo_clona(x1, num_cidades, num_veiculos);
	int* perturbado_fim_rotas = perturbado->fim_rotas;

	double random;
	int i, j,
	    index,
	    index_mutante,
	    index_perturbado,
	    rota_mutante,
	    rota_perturbada;
	
	int **rotas_mutantes = mutante->rotas,
	    **rotas_perturbadas = perturbado->rotas,
	    *posicoes_perturbadas_rotas = perturbado->posicoes[0],
	    *posicoes_perturbadas_cidades = perturbado->posicoes[1];
	    
	int cidade_mutante = rotas_mutantes[j_rand_rota][j_rand_componente];
	int cidade_perturbada;
	
	/* Perturbando o individuo com a cidade selecionada jrand, para garantir que o indivíduo perturbado seja diferente do indivíduo alvo. */
	if(j_rand_componente >= perturbado_fim_rotas[j_rand_rota]) {		
		individuo_remove_cidade(perturbado, cidade_mutante, 0);
		
		/* Adicionando na posicao j_rand. */
		index = perturbado_fim_rotas[j_rand_rota];
		
		rotas_perturbadas[j_rand_rota][index] = cidade_mutante;
		posicoes_perturbadas_rotas[cidade_mutante] = j_rand_rota; 
		posicoes_perturbadas_cidades[cidade_mutante] = index;
		
		perturbado_fim_rotas[j_rand_rota]++;
		cidades_fechadas[cidade_mutante] = 1;
	} else {
		cidade_perturbada = rotas_perturbadas[j_rand_rota][j_rand_componente];	
	
		/* Se as cidades forem iguais a perturbação não é efetuada. */
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
	
	for(i = 0; i < num_veiculos; i++){
		for(j = 0; j < mutante->fim_rotas[i]; j++){
			random = (double) rand() / RAND_MAX;
			
			if(random <= CR) {
				cidade_mutante = rotas_mutantes[i][j];
				
				/* Se a cidade não foi fechada a perturbação será feita. */
				if(!cidades_fechadas[cidade_mutante]){
					if(j >= perturbado_fim_rotas[i]){
						/* Individuo alvo não possui cidade no mesmo indice da componente selecionada. Então a cidade será adicionada ao final daquela lista. */
						individuo_remove_cidade(perturbado, cidade_mutante, 0);
					
						/* Adicionando na posicao selecionada. */
						index = perturbado_fim_rotas[i];
						
						rotas_perturbadas[i][index] = cidade_mutante;
						posicoes_perturbadas_rotas[cidade_mutante] = i; 
						posicoes_perturbadas_cidades[cidade_mutante] = index;
		
						perturbado_fim_rotas[i]++;
						cidades_fechadas[cidade_mutante] = 1;
					} else {
						cidade_perturbada = rotas_perturbadas[i][j];
						
						/* Se as cidades forem iguais a perturbação não é efetuada. */
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
	return perturbado;
}

void EvolucaoDiferencial_1(int** distancias, Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max, int melhor_solucao, int mutacao_rand, int crossover_binario) {
	Geracao *geracao = geracao_inicial(distancias, cidades, num_cidades, num_veiculos, capacidade_max),
	        *geracao2 = NULL;
	
	FILE *file_solucao = NULL,
	     *relatorio = NULL;
	
	if(IMPRESSAO_ARQUIVO) {
		file_solucao = fopen("solucao.vrp", "w");
		relatorio = fopen("relatorio.ed", "w");
		imprime_relatorio_arquivo(relatorio, geracao);
		fprintf(relatorio, "\n");
	} else {
		imprime_relatorio_terminal(geracao);
		printf("\n");
	}
	
	int qtd_geracoes = 1,
	    encontrou_melhor_solucao = 0,
	    melhor_fitness = geracao->melhor_solucao->custo;
	do {
		geracao2 = nova_geracao_1(geracao, distancias, cidades, num_cidades, num_veiculos, capacidade_max, mutacao_rand, crossover_binario);
		
		if(melhor_fitness != geracao2->melhor_solucao->custo){
			if(IMPRESSAO_ARQUIVO) {
				imprime_relatorio_arquivo(relatorio, geracao2);
				fprintf(relatorio, "\n");
			} else {
				imprime_relatorio_terminal(geracao2);
				printf("\n");
			}
			
			melhor_fitness = geracao2->melhor_solucao->custo;
		}
		
		geracao = geracao_libera(geracao, num_veiculos);
		geracao = geracao2;
		qtd_geracoes++;
		
		geracao_zera_clones(geracao);
		
		if(geracao->melhor_solucao != NULL && geracao->melhor_solucao->custo == melhor_solucao) {
			encontrou_melhor_solucao = 1;
		}
		
	}while (!encontrou_melhor_solucao && qtd_geracoes < NUM_MAX_GERACOES);
	

	if(IMPRESSAO_ARQUIVO) {
		if(qtd_geracoes == NUM_MAX_GERACOES){
			imprime_relatorio_arquivo(relatorio, geracao2);
			fprintf(relatorio, "\n");
		}
	
		if(geracao->melhor_solucao != NULL) {
			imprime_individuo_arquivo(file_solucao, geracao->melhor_solucao, num_veiculos, num_cidades);
		} else  fprintf(file_solucao, "\nNão houve solução viável.\n");
		
		printf("    Verifique o arquivo solucao.vrp no disco.\n");
		fclose(relatorio);
		fclose(file_solucao);
		
	} else {
		if(qtd_geracoes == NUM_MAX_GERACOES){
			imprime_relatorio_terminal(geracao2);
			printf("\n");
		}
	
		if(geracao->melhor_solucao != NULL) {
			imprime_individuo_terminal(geracao->melhor_solucao, num_veiculos, num_cidades);
		} else  printf("\nNão houve solução viável.\n");
	}

	geracao_zera_clones(geracao);
	if(geracao2->melhor_solucao->clonado){
		individuo_libera(geracao2->melhor_solucao, num_veiculos);
	}
	geracao = geracao_libera(geracao, num_veiculos);
	return;
}

void EvolucaoDiferencial_2(Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max, int melhor_solucao, int mutacao_rand, int crossover_binario) {
	return;
}

void EvolucaoDiferencial_2_rand_to_best(Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max, int melhor_solucao, int crossover_binario) {
	return;
}

/* Rodando
int main(int argc, char *argv[]) {
	F = atof(argv[1]);
	CR = atof(argv[2]);
	int semente = atoi(argv[4]);

	int num_cidades = 0,
	    num_veiculos = 0,
	    melhor_solucao = 0,
	    capacidade_max = 0,
	  //  escolha = escolha_tecnica(),
	    escolha = 2,
	    crossover_binario = escolha % 2; //Se a escolha é impar o crossover é binario.

	if(!crossover_binario) escolha--;
	
	if(escolha == 2) {
		printf("Técnica rand/1/exp\n");
	} else  printf("Técnica divergente!\n");
	
	char nome[] = "a";

// 	printf("    Escreva o nome do arquivo com os dados das cidades.\n    Nome: ");
//	scanf("%s", nome);
	printf("\n");

	FILE* leitura = fopen(argv[3], "r");
	if(leitura == NULL) {
		printf("    O arquivo %s não foi encontrado no disco.\n    Abortando...\n\n", nome);
		return 0;
	}
	
	fscanf(leitura, "%d %d %d %d", &num_veiculos, &melhor_solucao, &num_cidades, &capacidade_max);
	
	NP = 2 * num_cidades;
	
	Cidade cidades[num_cidades];
	arquivo_inicializa_cidades(cidades, leitura, num_cidades);
	
	int** distancias = matriz_custos_inicializa(cidades, num_cidades);

	fclose(leitura);
	
	printf("Semente: %d\n", semente); 
	srand( semente );
	
	switch(escolha) {//1 indica que o tipo de mutação é randomica (rand) e 0 utiliza o melhor individuo (best).
		case 1 :
			EvolucaoDiferencial_1(distancias, cidades, num_cidades, num_veiculos, capacidade_max, melhor_solucao, 1, crossover_binario); 
			break;
		case 3:
			EvolucaoDiferencial_1(distancias, cidades, num_cidades, num_veiculos, capacidade_max, melhor_solucao, 0, crossover_binario);
			break;
		case 5:
			EvolucaoDiferencial_2(cidades, num_cidades, num_veiculos, capacidade_max, melhor_solucao, 1, crossover_binario);
			break;
		case 7:
			EvolucaoDiferencial_2(cidades, num_cidades, num_veiculos, capacidade_max, melhor_solucao, 0, crossover_binario);
			break;
		case 9:
			EvolucaoDiferencial_2_rand_to_best(cidades, num_cidades, num_veiculos, capacidade_max, melhor_solucao, crossover_binario);
			break;
	}
	
	distancias = matriz_custos_libera(distancias, num_cidades);
	return 0;
}
*/

int main(int argc, char *argv[]) {
	F = atof(argv[1]);
	CR = atof(argv[2]);
	int semente = atof(argv[4]);
	
	printf("ED Contínua!\n\n");

	int num_cidades = 0,
	    num_veiculos = 0,
	    melhor_solucao = 0,
	    capacidade_max = 0,
	  //  escolha = escolha_tecnica(),
	    escolha = 2,
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
	
	char nome[] = "a";

// 	printf("    Escreva o nome do arquivo com os dados das cidades.\n    Nome: ");
//	scanf("%s", nome);

	FILE* leitura = fopen(argv[3], "r");
	if(leitura == NULL) {
		printf("    O arquivo %s não foi encontrado no disco.\n    Abortando...\n\n", nome);
		return 0;
	}
	
	fscanf(leitura, "%d %d %d %d", &num_veiculos, &melhor_solucao, &num_cidades, &capacidade_max);
	
	NP = 3 * num_cidades;
	printf("NP=%d\n", NP);
	
	Cidade cidades[num_cidades];
	arquivo_inicializa_cidades(cidades, leitura, num_cidades);
	
	int** distancias = matriz_custos_inicializa(cidades, num_cidades);

	fclose(leitura);
	
	printf("Semente: %d\n\n", semente); 
	srand( semente );
	
	switch(escolha) {//1 indica que o tipo de mutação é randomica (rand) e 0 utiliza o melhor individuo (best).
		case 1 :
			EvolucaoDiferencial_1(distancias, cidades, num_cidades, num_veiculos, capacidade_max, melhor_solucao, 1, crossover_binario); 
			break;
		case 3:
			EvolucaoDiferencial_1(distancias, cidades, num_cidades, num_veiculos, capacidade_max, melhor_solucao, 0, crossover_binario);
			break;
		case 5:
			EvolucaoDiferencial_2(cidades, num_cidades, num_veiculos, capacidade_max, melhor_solucao, 1, crossover_binario);
			break;
		case 7:
			EvolucaoDiferencial_2(cidades, num_cidades, num_veiculos, capacidade_max, melhor_solucao, 0, crossover_binario);
			break;
		case 9:
			EvolucaoDiferencial_2_rand_to_best(cidades, num_cidades, num_veiculos, capacidade_max, melhor_solucao, crossover_binario);
			break;
	}
	
	distancias = matriz_custos_libera(distancias, num_cidades);
	return 0;
}






