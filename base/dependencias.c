#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../HeadED/EvolucaoDiferencial.h"


Cidade* cidade_cria(int id, double x, double y){
	Cidade* cidade = (Cidade*) malloc(sizeof(Cidade));
	cidade->id = id;
	cidade->x = x;
	cidade->y = y;
//	cidade->demanda = 0;
	return cidade;
}

Cidade* cidade_libera(Cidade* cidade) {
	free(cidade);
	return NULL;
}

//Deixou de ter uso.
/*
int cidade_calcula_distancia(Cidade* c1, Cidade* c2){
	return sqrt( pow( (c1->x - c2->x), 2) + pow( (c1->y - c2->y), 2) ) + 0.5;
}
*/

int** matriz_custos_inicializa(Cidade* cidades, int num_cidades){
	int i, j, custo;
	    
	int** mat = (int**) malloc (num_cidades * sizeof(int*));
	for(i = 0; i < num_cidades; i++)
		mat[i] = (int*) malloc (num_cidades * sizeof(int));
	
	Cidade *c1, *c2;
	for(i = 1; i < num_cidades; i++){
		c1 = &cidades[i];
		for(j = i -1; j >= 0; j--){
			c2 = &cidades[j];
			custo = sqrt( pow((c1->x - c2->x), 2) + pow((c1->y - c2->y), 2) ) + 0.5;
			mat[i][j] = mat[j][i] = custo;
		}
	}
	
	for(i = 0; i < num_cidades; i++)
		mat[i][i] = 0;
		
	return mat;
}


int** matriz_custos_libera(int** distancias, int num_cidades){
	int i;
	for(i = 0; i < num_cidades; i++)
		free(distancias[i]);
	
	free(distancias);
	return NULL;
}
		
Individuo* individuo_inicializa(int num_cidades, int num_veiculos) {
	Individuo* individuo = (Individuo*) malloc(sizeof(Individuo));
	individuo->rotas = (int**) malloc(num_veiculos * sizeof(int*));
	
	int i;
	for(i = 0; i < num_veiculos; i++)
		individuo->rotas[i] = (int*) malloc(num_cidades * sizeof(int));

	individuo->posicoes =   (int**) malloc(2 * sizeof(int*));
	individuo->posicoes[0] = (int*) malloc(num_cidades * sizeof(int));
	individuo->posicoes[1] = (int*) malloc(num_cidades * sizeof(int));

	individuo->posicoes[0][0] = individuo->posicoes[1][0] = 0;

	individuo->fim_rotas = (int*) malloc (num_veiculos * sizeof(int));
	individuo->cargas_disponiveis = (int*) malloc (num_veiculos * sizeof(int));
	
	individuo->clonado = 0;
	return individuo;
}

Individuo* individuo_libera(Individuo* individuo, int num_veiculos) {
	int i;
	for(i = 0; i < num_veiculos; i++)
		free(individuo->rotas[i]);
		
	free(individuo->rotas);
	
	free(individuo->cargas_disponiveis);
	free(individuo->posicoes[0]);
	free(individuo->posicoes[1]);
	free(individuo->posicoes);
	
	free(individuo->fim_rotas);
	free(individuo);
	return NULL;
}

Individuo* individuo_gera_mais_esquerda(int** distancias, Cidade* cidades, int num_cidades, int capacidade_max, int num_veiculos){
	int carga,
	    random,
	    capacidade;
	    
	int cidades_visitadas[num_cidades],
	    cidades_fechadas[num_cidades];
	    
	memset(cidades_visitadas, 0, num_cidades*sizeof(int));
	memset(cidades_fechadas, 0,  num_cidades*sizeof(int));
	cidades_visitadas[0] = 1;
	cidades_fechadas[0] = 1;
	
	Individuo* individuo = individuo_inicializa(num_cidades, num_veiculos);
	int **rotas = individuo->rotas,
	    *fim_rotas = individuo->fim_rotas,
	    *posicoes_rotas = individuo->posicoes[0],
	    *posicoes_cidades = individuo->posicoes[1];
	
	int index = 0,
	    num_cidades_visitadas = 1,
	    num_cidades_fechadas = 1;
	
	int i, rotas_fechadas;
	for(rotas_fechadas = 0; rotas_fechadas < num_veiculos; rotas_fechadas++){

		capacidade = 0;
		do{
			do{
				random = (rand() % (num_cidades-1)) +1; //+1 pois 0 é o posto; raio 1-31.
			}while(cidades_visitadas[random]);

			carga = cidades[random].demanda;
			capacidade += carga;

			if(capacidade < capacidade_max) {
				rotas[rotas_fechadas][index] = random;
				posicoes_rotas[random] = rotas_fechadas;
				posicoes_cidades[random] = index;

				cidades_fechadas[random] = 1;
				num_cidades_fechadas++;
				index++;
				
			} else capacidade = capacidade - carga;
		
			cidades_visitadas[random] = 1;
			num_cidades_visitadas++;
			
		} while(num_cidades_visitadas < num_cidades);
		
		num_cidades_visitadas = 1;
		for(i = 1; i < num_cidades; i++){
			if(cidades_fechadas[i]){
				cidades_visitadas[i] = 1;
				num_cidades_visitadas++;
			} else cidades_visitadas[i] = 0;
		}

		fim_rotas[rotas_fechadas] = index;
		index = 0;
	}
	
	//Verificando viabilidade do individuo.
	if(num_cidades_fechadas == num_cidades){
		individuo_atualiza_atributos(individuo, capacidade_max, num_veiculos, distancias, cidades);
		return individuo;
	}
	
	/* Caso ainda existam cidades que não foram alocadas em alguma rota, elas serão inseridas na ultima rota */
	rotas_fechadas--;
	index = fim_rotas[rotas_fechadas];
	
	for(i = 1; i < num_cidades; i++){
		if(!cidades_fechadas[i]){
			rotas[rotas_fechadas][index] = i;
			posicoes_rotas[i] = rotas_fechadas;
			posicoes_cidades[i] = index;
			
			index++;
		}
	}
	
	fim_rotas[rotas_fechadas] = index;
	individuo_atualiza_atributos(individuo, capacidade_max, num_veiculos, distancias, cidades);
	return individuo;
}

Individuo* individuo_gera_mais_direita(int** distancias, Cidade* cidades, int num_cidades, int capacidade_max, int num_veiculos){
	int carga,
	    random,
	    capacidade;
	    
	int cidades_visitadas[num_cidades],
	    cidades_fechadas[num_cidades];
	    
	memset(cidades_visitadas, 0, num_cidades*sizeof(int));
	memset(cidades_fechadas, 0,  num_cidades*sizeof(int));
	cidades_visitadas[0] = 1;
	cidades_fechadas[0] = 1;
	
	Individuo* individuo = individuo_inicializa(num_cidades, num_veiculos);
	int **rotas = individuo->rotas,
	    *fim_rotas = individuo->fim_rotas,
	    *posicoes_rotas = individuo->posicoes[0],
	    *posicoes_cidades = individuo->posicoes[1];
	
	int index = 0,
	    num_cidades_visitadas = 1,
	    num_cidades_fechadas = 1;
	
	int i, rotas_fechadas;
	for(rotas_fechadas = num_veiculos -1; rotas_fechadas >= 0; rotas_fechadas--){
	
		capacidade = 0;
		do {
			do{
				random = (rand() % (num_cidades-1)) +1; //+1 pois 0 é o posto; raio 1-31.
			}while(cidades_visitadas[random]);

			carga = cidades[random].demanda;
			capacidade = capacidade + carga;

			if(capacidade < capacidade_max) {
				rotas[rotas_fechadas][index] = random;
				posicoes_rotas[random] = rotas_fechadas;
				posicoes_cidades[random] = index;

				cidades_fechadas[random] = 1;
				num_cidades_fechadas++;
				index++;

			} else capacidade = capacidade - carga;
		
			cidades_visitadas[random] = 1;
			num_cidades_visitadas++;
		} while(num_cidades_visitadas < num_cidades);
		
		num_cidades_visitadas = 1;
		for(i = 1; i < num_cidades; i++){
			if(cidades_fechadas[i]){
				cidades_visitadas[i] = 1;
				num_cidades_visitadas++;
				
			} else  cidades_visitadas[i] = 0;
		}

		fim_rotas[rotas_fechadas] = index;
		index = 0;
	}
	
	//Verificando viabilidade do individuo.
	if(num_cidades_fechadas == num_cidades){
		individuo_atualiza_atributos(individuo, capacidade_max, num_veiculos, distancias, cidades);
		return individuo;
	}
	
	/* Caso ainda existam cidades que não foram alocadas em alguma rota, elas serão inseridas na primeira rota */
	rotas_fechadas++;
	index = fim_rotas[rotas_fechadas];
	
	for(i = 1; i < num_cidades; i++){
		if(!cidades_fechadas[i]){
			rotas[rotas_fechadas][index] = i;
			posicoes_rotas[i] = rotas_fechadas;
			posicoes_cidades[i] = index;
			
			index++;
		}
	}
	
	fim_rotas[rotas_fechadas] = index;	
	individuo_atualiza_atributos(individuo, capacidade_max, num_veiculos, distancias, cidades);
	return individuo;
}
		
void individuo_atualiza_atributos(Individuo* individuo, int capacidade_max, int num_veiculos, int** distancias, Cidade* cidades){
	int **rotas = individuo->rotas;
	
	int *rota,
	    *fim_rotas = individuo->fim_rotas,
	    *cargas_disponiveis = individuo->cargas_disponiveis;
	     
	int custo,
	    capacidade,
	    componente,
	    cidade_atual,
	    cidade_anterior;
	    
	individuo->viavel = 1;
	individuo->custo = 0;
		
	int i, j,
	    fim_rota;
	for(i = 0; i < num_veiculos; i++) {
	
		rota = rotas[i];
		fim_rota = fim_rotas[i];
		capacidade = 0;
		for(j = 0; j < fim_rota; j++){
			componente = rota[j];
			capacidade += cidades[componente].demanda;
		}
		cargas_disponiveis[i] = capacidade_max - capacidade;
		
		if(capacidade > capacidade_max && individuo->viavel){
			individuo->viavel = 0;
			individuo->custo += PENALIDADE;
		}
		
		//Calculando o custo.
		cidade_anterior = 0;
		custo = 0;
		for(j = 0; j < fim_rota; j++){
			cidade_atual = rota[j]; 
			custo += distancias[cidade_anterior][cidade_atual];
			cidade_anterior = cidade_atual;
		}
		
		custo += distancias[cidade_anterior][0];
		individuo->custo += custo;
	}
	return;
}

/* Não são clonadas as cargas disponiveis */
Individuo* individuo_clona(Individuo* individuo, int num_cidades, int num_veiculos) {
	Individuo* clone = individuo_inicializa(num_cidades, num_veiculos);
	
	int **rotas_clone = clone->rotas,
	    **rotas_individuo = individuo->rotas;
	
	int *rota_clone,
	    *rota_individuo,
	    *clone_fim_rotas = clone->fim_rotas,
	    *individuo_fim_rotas = individuo->fim_rotas;
	    
	int *posicoes_clone_rotas = clone->posicoes[0],
	    *posicoes_clone_cidades = clone->posicoes[1],
	    *posicoes_individuo_rotas = individuo->posicoes[0],
	    *posicoes_individuo_cidades = individuo->posicoes[1];
	    
	int i, j,
	    fim_rota = 0;
	for(i = 0; i < num_veiculos; i++) {
		rota_clone = rotas_clone[i];
		rota_individuo = rotas_individuo[i];
		clone_fim_rotas[i] = individuo_fim_rotas[i];
		
		fim_rota = individuo_fim_rotas[i];
		for(j = 0; j < fim_rota; j++)
			rota_clone[j] = rota_individuo[j];
	}
	
	for(j = 0; j < num_cidades; j++){
		posicoes_clone_rotas[j] = posicoes_individuo_rotas[j];
		posicoes_clone_cidades[j] = posicoes_individuo_cidades[j];
	}

	clone->custo = individuo->custo;
	clone->viavel = individuo->viavel;
	return clone;
}

/* Deve ser inserido em outra rota */
void individuo_insere_cidade(Individuo* individuo, int cidade, int carga, int nova_posicao, int nova_rota){
	int *posicoes_cidades = individuo->posicoes[1],
	    *rota = individuo->rotas[nova_rota],
	     posicao = individuo->fim_rotas[nova_rota];
	
	int cidade_selecionada;
	while(posicao > nova_posicao){
		cidade_selecionada = rota[posicao -1];
		rota[posicao] = cidade_selecionada;
		posicoes_cidades[cidade_selecionada]++;
		posicao--;
	}
	
	individuo->posicoes[0][cidade] = nova_rota;
	rota[posicao] = cidade;
	posicoes_cidades[cidade] = posicao;
	individuo->fim_rotas[nova_rota]++;
	
	individuo->cargas_disponiveis[nova_rota] -= carga; 
	return;
}

void individuo_reinsere_cidade_rota(Individuo* individuo, int cidade, int novo_index){
	int *posicoes_cidades = individuo->posicoes[1],
	    *rota = individuo->rotas[ individuo->posicoes[0][cidade] ],
	     posicao = posicoes_cidades[cidade];

	int cidade_selecionada;
	if(novo_index < posicao){
		while(posicao > novo_index){
			cidade_selecionada = rota[posicao -1];
			rota[posicao] = cidade_selecionada;
			posicoes_cidades[cidade_selecionada]++;
			posicao--;
		}
		
		rota[posicao] = cidade;
		posicoes_cidades[cidade] = posicao;
		return;
	}
	
	novo_index--;
	while(posicao < novo_index){
		cidade_selecionada = rota[posicao +1];
		rota[posicao] = cidade_selecionada;
		posicoes_cidades[cidade_selecionada]--;
		posicao++;
	}
	
	rota[posicao] = cidade;
	posicoes_cidades[cidade] = posicao;
	return;
}
/* Sem uso
void individuo_insere_cidade_fim_rota(Individuo* individuo, int cidade, int carga, int rota){
	int index = individuo->fim_rotas[rota];
	individuo->fim_rotas[rota]++;

	individuo->cargas_disponiveis[rota] -= carga;
	individuo->rotas[rota][index] = cidade;
	individuo->posicoes[0][cidade] = rota;
	individuo->posicoes[1][cidade] = index;
	return;
}*/

/* Se as cidades estiverem na mesma rota, passar as cargas como 0 pois não mudarão */
void individuo_troca_cidades(Individuo* individuo, int cidade1, int carga1, int cidade2, int carga2){
	int **rotas = individuo->rotas,
	    *posicoes_rotas = individuo->posicoes[0],
	    *posicoes_cidades = individuo->posicoes[1];
	
	int rota1 = posicoes_rotas[cidade1],
	    posicao1 = posicoes_cidades[cidade1],
	    rota2 = posicoes_rotas[cidade2],
	    posicao2 = posicoes_cidades[cidade2];
	
	rotas[rota2][posicao2] = cidade1;
	rotas[rota1][posicao1] = cidade2;
	
	posicoes_rotas[cidade2] = rota1;
	posicoes_cidades[cidade2] = posicao1;
	individuo->cargas_disponiveis[rota1] = individuo->cargas_disponiveis[rota1] + carga1 - carga2;

	posicoes_rotas[cidade1] = rota2;
	posicoes_cidades[cidade1] = posicao2;
	individuo->cargas_disponiveis[rota2] = individuo->cargas_disponiveis[rota2] - carga1 + carga2;

	return;
}

void individuo_remove_cidade(Individuo* individuo, int cidade, int carga){
	int *posicoes_cidades = individuo->posicoes[1],
	     num_rota = individuo->posicoes[0][cidade],
	     fim_rota = individuo->fim_rotas[num_rota],
	     index = posicoes_cidades[cidade];
	
	individuo->cargas_disponiveis[num_rota] += carga;
	individuo->fim_rotas[num_rota]--;
	
	//Se a cidade a ser removida é a última cidade da rota.
	if(index +1 == fim_rota)
		return;
	
	int *rota = individuo->rotas[num_rota],
	     nova_cidade = 0;
	while(index < fim_rota -1){
		nova_cidade =  rota[index +1];
		rota[index] = nova_cidade;
		posicoes_cidades[nova_cidade] = index;
		index++;
	}
	return;
}



