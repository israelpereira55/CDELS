#ifndef ARQUIVO_H_
#define ARQUIVO_H_

#include <stdio.h>

#include "dependences.h"
#include "../metaheuristic/differential_evolution.h"


int escolha_tecnica();

void arquivo_inicializa_cidades(Customer* customers, FILE* leitura, int customers_num);

void cidades_atualiza_demanda(Customer* customers, FILE* leitura, int customers_num);


void imprime_individual_terminal(Individual* individual, int vehicles_num, int customers_num);

void imprime_relatorio_terminal(Generation* generation);


void imprime_individual_arquivo(FILE* arquivo, Individual* individual, int vehicles_num, int customers_num);

void imprime_relatorio_arquivo(FILE* arquivo, Generation* generation);


//Apenas para testes.
void testa_individual(Individual* individual, Customer* customers, int customers_num, int vehicles_num); 


#endif /*ARQUIVO_H_*/
