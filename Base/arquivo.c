#include <stdio.h>
#include <stdlib.h>

#include "dependencias.h"

#include "../HeadED/EvolucaoDiferencial.h"


int escolha_tecnica(){
	int escolha = 0;

	do {
		printf( "=================================================================== \n"
			"			Evolução Diferencial.\n"
			"===================================================================\n\n"
		
		"Escolha a técnica:\n" 
		"    1 - ED/rand/1/bin\n" 
		"    2 - ED/rand/1/exp\n" 
		"    3 - ED/best/1/bin\n" 
		"    4 - ED/best/1/exp\n" 
		"    5 - ED/rand/2/bin\n" 
		"    6 - ED/rand/2/exp\n" 
		"    7 - ED/best/2/bin\n" 
		"    8 - ED/best/2/exp\n" 
		"    9 - ED/rand-to-best/2/bin\n" 
		"    10 - ED/rand-to-best/2/exp\n\n"
		"    Técnica: ");
	
		scanf("%d", &escolha);
		if(escolha < 1 || escolha > 10) {
			printf("    Escolha inválida!\n\n");
			escolha = -1;
		}
	}while(escolha == -1);
	
	printf("\n");
	return escolha;
}

void cidades_atualiza_demanda(Cidade* cidades, FILE* leitura, int num_cidades) {
	int i,
	    id,
	    demanda;
	    
	for(i = 0; i < num_cidades; i++){
		fscanf (leitura, "%d %d", &id, &demanda);
		cidades[i].demanda = demanda;
	}
	
	return;
}

void arquivo_inicializa_cidades(Cidade* cidades, FILE* leitura, int num_cidades){
	Cidade* cidade = NULL;
	
	double x, y;
	int i, id;
	for(i = 0; i < num_cidades; i++){
		fscanf (leitura, "%d %lf %lf", &id, &x, &y);  
		cidade = &cidades[i];
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
	
	cidades_atualiza_demanda(cidades, leitura, num_cidades);
	return;
}

void imprime_individuo_terminal(Individuo* individuo, int num_veiculos, int num_cidades) {

	if(individuo->viavel){
		printf("Individuo viavel\n");
	} else  printf("Individuo inviavel\n");
	
	printf("Custo: %d\n\n", individuo->custo);
	
	int **rotas = individuo->rotas,
	     *rota = NULL,
	     *fim_rotas = individuo->fim_rotas,
	      fim_rota;
	
	int i, j;
	for(i = 0; i < num_veiculos; i++){
		printf("Veiculo #%d: ", i+1);
		fim_rota = fim_rotas[i];
		rota = rotas[i];
		
		for(j = 0; j < fim_rota; j++)
			printf("%d ", rota[j] +1);
		
		printf("\n");
	}
	return;
}

void imprime_individuo_arquivo(FILE* arquivo, Individuo* individuo, int num_veiculos, int num_cidades) {

	if(individuo->viavel){
		fprintf(arquivo, "Individuo viavel\n");
	} else  fprintf(arquivo, "Individuo inviavel\n");
	
	fprintf(arquivo, "Custo: %d\n\n", individuo->custo);
	
	int **rotas = individuo->rotas,
	     *rota = NULL,
	     *fim_rotas = individuo->fim_rotas,
	      fim_rota;

	int i, j;	      
	for(i = 0; i < num_veiculos; i++){
		fprintf(arquivo, "Veiculo #%d: ", i+1);
		fim_rota = fim_rotas[i];
		rota = rotas[i];
		
		for(j = 0; j < fim_rota; j++)
			fprintf(arquivo, "%d ", rota[j] +1);
		
		fprintf(arquivo, "\n");
	}
	return;
}

void imprime_relatorio_terminal(Geracao* geracao) {
	printf("Geração: %d\n", geracao->id);

	if(geracao->melhor_solucao != NULL) {
		printf("Melhor solução viável: %d\n", geracao->melhor_solucao->custo);
	} else  printf("Melhor solução viável: VAZIO\n");
	
	printf("Número de soluções viáveis: %d.\n", geracao->num_solucoes_viaveis);
	return;
}

void imprime_relatorio_arquivo(FILE* arquivo, Geracao* geracao) {
	fprintf(arquivo, "Geração: %d\n", geracao->id);

	if(geracao->melhor_solucao != NULL) {
		fprintf(arquivo, "Melhor solução viável: %d\n", geracao->melhor_solucao->custo);
	} else  fprintf(arquivo, "Melhor solução viável: VAZIO\n");
	
	fprintf(arquivo, "Número de soluções viáveis: %d.\n", geracao->num_solucoes_viaveis);
	return;
}

void testa_individuo(Individuo* individuo, Cidade* cidades, int num_cidades, int num_veiculos){
	imprime_individuo_terminal(individuo, num_veiculos, num_cidades);
	printf("\n");
		
	int cidades_visitadas[num_cidades];
	
	printf("Indices: ");
	
	int i = 1;
	while(i < num_cidades +1){
		cidades_visitadas[i] = 0;
		i++;
		
		printf("%d ", (i-2)%10 );
	}
	
	printf("\n\n");
	
	int j = 0;
	printf("Rotas:   ");
	for(i = 0; i < num_cidades; i++){
			printf("%d ", individuo->posicoes[0][i]);
	} 
	
	printf("\nCidades: ");
	for(i = 0; i < num_cidades; i++){
		printf("%d ", individuo->posicoes[1][i]);
	} 
	
	printf("\n\n");
	
	for(i = 0; i < num_veiculos; i++){
		printf("FIM DA ROTA %d: %d\n", i+1, individuo->fim_rotas[i]);
	}
	
	printf("\n");
	
	int  carga;
	for(i = 0; i < num_veiculos; i++){
		carga = 0;
		for(j = 0; j < individuo->fim_rotas[i]; j++)
			carga += cidades[individuo->rotas[i][j]].demanda;
			
		printf("CARGA DA ROTA %d: %d\n", i, carga);
	}
	
	
	for(i = 0; i < num_veiculos; i++){
		for(j = 0; j < individuo->fim_rotas[i]; j++){
			cidades_visitadas[ individuo->rotas[i][j] ]++;
		}
	}
	
	int cont = 0;
	for(i = 1; i < num_cidades; i++){
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
	
	for(i = 0; i < num_veiculos; i++){	
		if(individuo->fim_rotas[i] == 0){
			printf("A ROTA %d ESTA VAZIA!\n", i);
		}
	}
	
	for(i = 1; i < num_cidades; i++){
		if( individuo->rotas[ individuo->posicoes[0][i] ][ individuo->posicoes[1][i] ] != i){
				printf("POSICAO DA CIDADE %d ERRADA!\n", i+1);
		}
	} 
	
	return;
}
