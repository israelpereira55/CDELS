#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include "dependences.h"

/* Uma iteração do Swap two points ou Flip, referem-se ao movimento completo, já uma iteração do drop one points refere-se 
 * a troca de uma customer.
 */

#define IT_SEM_MELHORA_STP 50
#define IT_SEM_MELHORA_DOP 50
#define IT_SEM_MELHORA_FLIP 50


void local_search(Individual* trial, int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max);


/* Best improvement version of two swap (TODO: confirm)
 */
void two_swap(Individual* individual, int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max);

void strong_drop_one_point(Individual* individual, int** distances, Customer* customers, int vehicles_num, int customers_num, int capacity_max);

int drop_one_point_infeasible(Individual* individual, int** distances, Customer* customers, int vehicles_num, int customers_num, int capacity_max);


/* @deprecated
 */ 
void swap_two_points(Individual* individual, int** distances, Customer* customers, int customers_num, int vehicles_num, int capacity_max);

/* @deprecated
 */ 
void drop_one_point(Individual* individual, int** distances, Customer* customers, int vehicles_num, int customers_num, int capacity_max);

/* Flip: Selecionará uma customer aleatória de cada rota e irá inserir aquela customer na melhor posição da rota que a possui.
 *
 * Retorno: 
 *   - 0 caso não haja melhora.
 *   - O novo custo do indivíduo, caso tenha obtido melhora.
 */
void flip(Individual* individual, int** distances, Customer* customers, int vehicles_num, int customers_num);


void reinsert_customer_best_position_in_another_route(Individual* individual, int** distances, int customer, int load, int new_route_idx);

/* Retorno:
 *   - 0 se não houve melhora.
 *   - 1 se houve melhora. 
 */
int reinsert_customer_best_position_in_another_route_if_improves(Individual* individual, int** distances, int customer, int load, int new_route_idx);

#endif /*BUSCALOCAL_H_*/
