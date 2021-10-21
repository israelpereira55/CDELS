#ifndef ARQUIVO_H_
#define ARQUIVO_H_

#include <stdio.h>

#include "dependencias.h"
#include "../HeadED/EvolucaoDiferencial.h"


int escolha_tecnica();

void arquivo_inicializa_cidades(Cidade* cidades, FILE* leitura, int num_cidades);

void cidades_atualiza_demanda(Cidade* cidades, FILE* leitura, int num_cidades);


void imprime_individuo_terminal(Individuo* individuo, int num_veiculos, int num_cidades);

void imprime_relatorio_terminal(Geracao* geracao);


void imprime_individuo_arquivo(FILE* arquivo, Individuo* individuo, int num_veiculos, int num_cidades);

void imprime_relatorio_arquivo(FILE* arquivo, Geracao* geracao);


//Apenas para testes.
void testa_individuo(Individuo* individuo, Cidade* cidades, int num_cidades, int num_veiculos); 


#endif /*ARQUIVO_H_*/
