#ifndef BUSCALOCAL_H_
#define BUSCALOCAL_H_

#include "dependencias.h"

/* Uma iteração do Swap two points ou Flip, referem-se ao movimento completo, já uma iteração do drop one points refere-se 
 * a troca de uma cidade.
 */

#define IT_SEM_MELHORA_STP 50
#define IT_SEM_MELHORA_DOP 50
#define IT_SEM_MELHORA_FLIP 50


void Busca_Local(Individuo* perturbado, int** distancias, Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max);


void two_opt(Individuo* individuo, int** distancias, Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max);

void swap_two_points(Individuo* individuo, int** distancias, Cidade* cidades, int num_cidades, int num_veiculos, int capacidade_max);

void drop_one_point_viavel(Individuo* individuo, int** distancias, Cidade* cidades, int num_veiculos, int num_cidades, int capacidade_max);

void strong_drop_one_point_viavel(Individuo* individuo, int** distancias, Cidade* cidades, int num_veiculos, int num_cidades, int capacidade_max);

int drop_one_point_inviavel(Individuo* individuo, int** distancias, Cidade* cidades, int num_veiculos, int num_cidades, int capacidade_max);

/* Flip: Selecionará uma cidade aleatória de cada rota e irá inserir aquela cidade na melhor posição da rota que a possui.
 *
 * Retorno: 
 *   - 0 caso não haja melhora.
 *   - O novo custo do indivíduo, caso tenha obtido melhora.
 */
void flip(Individuo* individuo, int** distancias, Cidade* cidades, int num_veiculos, int num_cidades);


void reinsere_cidade_melhor_posicao_outra_rota(Individuo* individuo, int** distancias, int cidade, int carga, int nova_rota);

/* Retorno:
 *   - 0 se não houve melhora.
 *   - 1 se houve melhora. 
 */
int reinsere_cidade_melhor_posicao_outra_rota_caso_melhore(Individuo* individuo, int** distancias, int cidade, int carga, int nova_rota);

#endif /*BUSCALOCAL_H_*/
