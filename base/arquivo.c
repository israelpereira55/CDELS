#include <stdio.h>
#include <stdlib.h>

#include "dependences.h"
#include "../metaheuristic/differential_evolution.h"


int escolha_tecnica(){
	int escolha = 0;

	do {
		printf( "=================================================================== \n"
			"			CDELS for CVRP\n"
			"===================================================================\n\n"
		
		"Escolha a técnica:\n" 
		"    1 - DE/rand/1/bin\n" 
		"    2 - DE/rand/1/exp\n" 
		"    3 - DE/best/1/bin\n" 
		"    4 - DE/best/1/exp\n" 
		"    Técnica: ");
	
		scanf("%d", &escolha);
		if(escolha < 1 || escolha > 4) {
			printf("    Escolha inválida!\n\n");
			escolha = -1;
		}
	}while(escolha == -1);
	
	printf("\n");
	return escolha;
}

void cidades_atualiza_demanda(Customer* customers, FILE* leitura, int customers_num) {
	int i,
	    id,
	    demanda;
	    
	for(i = 0; i < customers_num; i++){
		fscanf (leitura, "%d %d", &id, &demanda);
		customers[i].demanda = demanda;
	}
	
	return;
}

void arquivo_inicializa_cidades(Customer* customers, FILE* leitura, int customers_num){
	Customer* cidade = NULL;
	
	double x, y;
	int i, id;
	for(i = 0; i < customers_num; i++){
		fscanf (leitura, "%d %lf %lf", &id, &x, &y);  
		cidade = &customers[i];
		cidade->id = id;
		cidade->x = x;
		cidade->y = y;
	}
	
	int qtd_linhas = 2;
	char c = 0;
	while(qtd_linhas > 0) {
		c = fgetc(leitura);
		if(c == '\n') {
			qtd_linhas--;
		}
	}
	
	cidades_atualiza_demanda(customers, leitura, customers_num);
	return;
}

void imprime_individual_terminal(Individuo* individual, int vehicles_num, int customers_num) {

	if(individual->viavel){
		printf("Individuo viavel\n");
	} else  printf("Individuo inviavel\n");
	
	printf("Custo: %d\n\n", individual->custo);
	
	int **rotas = individual->rotas,
	     *rota = NULL,
	     *fim_rotas = individual->fim_rotas,
	      fim_rota;
	
	int i, j;
	for(i = 0; i < vehicles_num; i++){
		printf("Veiculo #%d: ", i+1);
		fim_rota = fim_rotas[i];
		rota = rotas[i];
		
		for(j = 0; j < fim_rota; j++)
			printf("%d ", rota[j] +1);
		
		printf("\n");
	}
	return;
}

void imprime_individual_arquivo(FILE* arquivo, Individuo* individual, int vehicles_num, int customers_num) {

	if(individual->viavel){
		fprintf(arquivo, "Individuo viavel\n");
	} else  fprintf(arquivo, "Individuo inviavel\n");
	
	fprintf(arquivo, "Custo: %d\n\n", individual->custo);
	
	int **rotas = individual->rotas,
	     *rota = NULL,
	     *fim_rotas = individual->fim_rotas,
	      fim_rota;

	int i, j;	      
	for(i = 0; i < vehicles_num; i++){
		fprintf(arquivo, "Veiculo #%d: ", i+1);
		fim_rota = fim_rotas[i];
		rota = rotas[i];
		
		for(j = 0; j < fim_rota; j++)
			fprintf(arquivo, "%d ", rota[j] +1);
		
		fprintf(arquivo, "\n");
	}
	return;
}

void imprime_relatorio_terminal(Generation* generation) {
	printf("Geração: %d\n", generation->id);

	if(generation->melhor_solucao != NULL) {
		printf("Melhor solução viável: %d\n", generation->melhor_solucao->custo);
	} else  printf("Melhor solução viável: VAZIO\n");
	
	printf("Número de soluções viáveis: %d.\n", generation->num_solucoes_viaveis);
	return;
}

void imprime_relatorio_arquivo(FILE* arquivo, Generation* generation) {
	fprintf(arquivo, "Geração: %d\n", generation->id);

	if(generation->melhor_solucao != NULL) {
		fprintf(arquivo, "Melhor solução viável: %d\n", generation->melhor_solucao->custo);
	} else  fprintf(arquivo, "Melhor solução viável: VAZIO\n");
	
	fprintf(arquivo, "Número de soluções viáveis: %d.\n", generation->num_solucoes_viaveis);
	return;
}

void testa_individual(Individuo* individual, Customer* customers, int customers_num, int vehicles_num){
	imprime_individual_terminal(individual, vehicles_num, customers_num);
	printf("\n");
		
	int cidades_visitadas[customers_num];
	
	printf("Indices: ");
	
	int i = 1;
	while(i < customers_num +1){
		cidades_visitadas[i] = 0;
		i++;
		
		printf("%d ", (i-2)%10 );
	}
	
	printf("\n\n");
	
	int j = 0;
	printf("Rotas:   ");
	for(i = 0; i < customers_num; i++){
			printf("%d ", individual->posicoes[0][i]);
	} 
	
	printf("\nCidades: ");
	for(i = 0; i < customers_num; i++){
		printf("%d ", individual->posicoes[1][i]);
	} 
	
	printf("\n\n");
	
	for(i = 0; i < vehicles_num; i++){
		printf("FIM DA ROTA %d: %d\n", i+1, individual->fim_rotas[i]);
	}
	
	printf("\n");
	
	int  carga;
	for(i = 0; i < vehicles_num; i++){
		carga = 0;
		for(j = 0; j < individual->fim_rotas[i]; j++)
			carga += customers[individual->rotas[i][j]].demanda;
			
		printf("CARGA DA ROTA %d: %d\n", i, carga);
	}
	
	
	for(i = 0; i < vehicles_num; i++){
		for(j = 0; j < individual->fim_rotas[i]; j++){
			cidades_visitadas[ individual->rotas[i][j] ]++;
		}
	}
	
	int cont = 0;
	for(i = 1; i < customers_num; i++){
		if( !cidades_visitadas[i]){
			printf("A CIDADE %d ESTA FALTANDO.\n", i+1);
		}
		
		if(cidades_visitadas[i] > 1){
			printf("COPIA DA CIDADE %d, %d vzs\n", i+1, cidades_visitadas[i]);
			
		}
		
		if(cidades_visitadas[i]){
			cont++;
		}
	}
	
	printf("QUANTIDADE DE CIDADES %d\n\n", cont);
	
	for(i = 0; i < vehicles_num; i++){	
		if(individual->fim_rotas[i] == 0){
			printf("A ROTA %d ESTA VAZIA!\n", i);
		}
	}
	
	for(i = 1; i < customers_num; i++){
		if( individual->rotas[ individual->posicoes[0][i] ][ individual->posicoes[1][i] ] != i){
				printf("POSICAO DA CIDADE %d ERRADA!\n", i+1);
		}
	} 
	
	return;
}
