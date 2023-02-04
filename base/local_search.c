#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include "dependences.h"
#include "local_search.h"

#include "../metaheuristic/differential_evolution.h"

/* Determina a customer_preceding e a customer_successor da customer determinada pela component.
 *
 * Variáveis:
 *   - (int)  component, route_end, customer_preceding, customer_successor.
 *   - (int*) route.
 */
#define get_customer_preceding_and_successor(component, route, route_end, customer_preceding, customer_successor) \
    if (component == 0) { \
        customer_preceding = 0; \
        customer_successor = route[1]; \
    } else if (component == route_end -1) { \
        customer_preceding = route[component -1]; \
        customer_successor = 0; \
    } else{ \
        customer_preceding = route[component -1]; \
        customer_successor = route[component +1]; \
    }


//TODO: delete this function? 
/* Determina a customer_preceding e a customer_successor da customer determinada pela component.
 * Mesmo código da função superior, com a diferença que se a component estiver na ultima posição da route,
 * o fim da route é decrementado. (Usado quando é preciso remover a customer de uma route para outra).
 *
 * Variáveis:
 *   - (int)  component, route_end, customer_preceding, customer_successor.
 *   - (int*) route.
 */ 
#define get_customer_preceding_and_successor_dec_route_end(component, route, route_end, customer_preceding, customer_successor) \
    if (component == 0) { \
        customer_preceding = 0; \
        customer_successor = route[1]; \
    } else if (component == route_end -1) { \
        customer_preceding = route[component -1]; \
        customer_successor = 0; \
        route_end--; \
    } else{ \
        customer_preceding = route[component -1]; \
        customer_successor = route[component +1]; \
    }


/* Calcula o cost do indivíduo com a troca da customer_old pela customer_new.
 *
 * Variáveis:
 *   - (int)   cost, customer_preceding, customer_successor, customer_old, customer_new.
 *   - (int**) distances.
 */
#define calculate_swap_cost_exclusive(cost, distances, customer_preceding, customer_successor, customer_old, customer_new) \
    cost   - distances[customer_preceding][customer_old]  \
        - distances[customer_old][customer_successor] \
        + distances[customer_preceding][customer_new]    \
        + distances[customer_new][customer_successor]


/* Calcula o cost do indivíduo com a remoção da customer.
 * Não deve ser usado caso a route esteja vazia, pois é adicionado o cost da customer anterior a customer posterior,
 * mas mesmo que neste caso o valor ainda será correto, será menos eficiente.
 *
 * Variáveis:
 *   - (int)   cost, customer_preceding, customer_successor, customer.
 *   - (int**) distances.
 */ 
#define calculate_customer_remotion(cost, distances, customer_preceding, customer_successor, customer) \
    cost   - distances[customer_preceding][customer] \
        - distances[customer][customer_successor] \
        + distances[customer_preceding][customer_successor]


/* Calcula o cost do indivíduo com adição da customer.
 * Não deve ser usado caso a route esteja vazia, pois é removido o cost da customer anterior a customer posterior.
 *
 * Variáveis:
 *   - (int)   cost, customer_preceding, customer_successor, customer.
 *   - (int**) distances.
 */
#define calculate_customer_insertion(cost, distances, customer_preceding, customer_successor, customer) \
    cost   + distances[customer_preceding][customer] \
        + distances[customer][customer_successor] \
        - distances[customer_preceding][customer_successor]


/* Calcula o cost do indivíduo com a troca da customeri pela customerj.
 */
#define calculate_swap_cost_inclusive(routes, distances, cost_new, cost, i, j, customeri, routei, icustomer_preceding, icustomer_successor, customerj, routej, jcustomer_preceding, jcustomer_successor) \
    if (i == j-1) { \
    cost_new = cost - distances[icustomer_preceding][customeri] \
        - distances[customerj][jcustomer_successor] \
        + distances[customerj][icustomer_preceding] \
        + distances[customeri][jcustomer_successor]; \
    } else { \
    cost_new = cost + calculate_swap_cost_exclusive(0, distances, icustomer_preceding, icustomer_successor, customeri, customerj) \
        +  calculate_swap_cost_exclusive(0, distances, jcustomer_preceding, jcustomer_successor, customerj, customeri); \
    }



void local_search(Individual* trial, int** distances, Customer* customers, int customers_num, int vehicles_num, int capacidade_max) {
    int it_no_improvement_cnt = 0;
    int original_cost = trial->cost;
    
    do {
        two_swap(trial, distances, customers, customers_num, vehicles_num, capacidade_max);
        if (original_cost > trial->cost) {
            original_cost = trial->cost;
            it_no_improvement_cnt = 0;
        } else {
            it_no_improvement_cnt++;
        }

        if (it_no_improvement_cnt > 1) break;
        
        strong_drop_one_point(trial, distances, customers, vehicles_num, customers_num, capacidade_max);
        if (original_cost > trial->cost) {
            original_cost = trial->cost;
            it_no_improvement_cnt = 0;
        } else {
            it_no_improvement_cnt++;
        }

        if (!trial->feasible) {
            drop_one_point_infeasible(trial, distances, customers, vehicles_num, customers_num, capacidade_max);
            if (trial->feasible) {
                it_no_improvement_cnt = 0;
            }
        }
    } while (it_no_improvement_cnt < 2);
    
    return;
}

void two_swap(Individual* individual, int** distances, Customer* customers, int customers_num, int vehicles_num, int capacidade_max) {
    int i, j,
        routei, routej,
        load, loadj,
        customeri, customerj,
        route_endi, route_endj,
        icustomer_preceding, icustomer_successor,
        jcustomer_preceding, jcustomer_successor;
     
    int *route;
    
    int cost_new,
        original_cost = individual->cost;
     
    int it_got_improvement = 0;
        
    for (routei = 0; routei < vehicles_num; routei++) {
    
        route_endi = individual->routes_end[routei];
        for (i = 0; i < route_endi; i++) {
RESTART_MOVEMENT_2SWAP:
            customeri = individual->routes[routei][i];
            load = customers[customeri].demand;
            
            for (routej = routei; routej < vehicles_num; routej++) {

                route_endj = individual->routes_end[routej];
                if (routei == routej) {
                    j = i+1;
                } else {
                    j = 0;
                }
                
                for (; j < route_endj; j++) {
                    /*Verificando se a troca é viável. Caso os dois pontos estejam na mesma route o movimento não inviabilizará o indivíduo. */
                    
                    customerj = individual->routes[routej][j];
                    loadj = customers[customerj].demand;
            
                    if (routei != routej) {
                        if ((individual->capacities_free[routei] + load < loadj) || (individual->capacities_free[routej] + loadj < load)) {
                            continue;
                        } else {
                            route = individual->routes[routei];
                            route[route_endi] = 0;
                            get_customer_preceding_and_successor(i, route, route_endi, icustomer_preceding, icustomer_successor); /* Macro */
                        
                            route = individual->routes[routej];
                            route[route_endj] = 0;
                            get_customer_preceding_and_successor(j, route, route_endj, jcustomer_preceding, jcustomer_successor);
                        
                            cost_new = original_cost + calculate_swap_cost_exclusive(0, distances, icustomer_preceding, icustomer_successor, customeri, customerj)
                                            + calculate_swap_cost_exclusive(0, distances, jcustomer_preceding, jcustomer_successor, customerj, customeri);
                            
                            if (cost_new < original_cost) {
                                original_cost = cost_new;
                                individual_swap_customers(individual, customeri, load, customerj, loadj);
                                
                                it_got_improvement = 1; 
                            }
                        }
                    } else {
                        route = individual->routes[routei];
                        get_customer_preceding_and_successor(i, route, route_endi, icustomer_preceding, icustomer_successor); /* Macro */
                    
                        route = individual->routes[routej];
                        get_customer_preceding_and_successor(j, route, route_endj, jcustomer_preceding, jcustomer_successor); /* Macro */

                        calculate_swap_cost_inclusive(individual->routes, distances, cost_new, original_cost, i, j, customeri, routei, icustomer_preceding, icustomer_successor, customerj, routej, jcustomer_preceding, jcustomer_successor);

                        if (cost_new < original_cost) {
                            original_cost = cost_new;
                            individual_swap_customers(individual, customeri, 0, customerj, 0); /* Como estão na mesma route, as cargas não mudarão */
                
                            it_got_improvement = 1;
                        }
                    }
                    
                    if (it_got_improvement) {
                        it_got_improvement = 0;
                        goto RESTART_MOVEMENT_2SWAP;
                    }
                }
            }
        }
    }

    if (!individual->feasible) {
        i = 0;
        while (i < vehicles_num && individual->capacities_free[i] >=0) i++;

        if (i == vehicles_num) {
            individual->cost = original_cost - PENALTY;
            individual->feasible = 1;
            return;
        }
    }
    
    individual->cost = original_cost;
    return;
}


/* @deprecated
 */ 
void swap_two_points(Individual* individual, int** distances, Customer* customers, int customers_num, int vehicles_num, int capacidade_max) {
    int i,
        load,
        fim_rota;

    int capacities_free[vehicles_num];

    int *route = NULL,
        *posicoes_rota = individual->positions[0],
        *posicoes_cidades = individual->positions[1];

    int rota_original = 0;
    do {
        route = individual->routes[rota_original];
        fim_rota = individual->routes_end[rota_original];

        load = 0;
        for (i = 0; i < fim_rota; i++)
            load = load + customers[ route[i] ].demand;

        capacities_free[rota_original] = capacidade_max - load;
        rota_original++;
    } while (rota_original < vehicles_num);

    int customer,
        nova_rota,
        cost_new,
        nova_carga,
        componente,
        nova_cidade,
        nova_componente,
        cidade_anterior,
        cidade_posterior;

    int houve_melhora,
        it_sem_melhora = 0,
        original_cost = individual->cost;
    do {
        customer = 1;
        houve_melhora = 0;
        do {
            rota_original = posicoes_rota[customer];
            componente = posicoes_cidades[customer];

            /* Obtendo a nova customer aleatória. */
            do {
                nova_rota = rand() % vehicles_num;
            }while (nova_rota == rota_original || individual->routes_end[nova_rota] == 0);

            nova_componente = rand() % individual->routes_end[nova_rota];
            nova_cidade = individual->routes[nova_rota][nova_componente];
            nova_carga = customers[nova_cidade].demand;
            load = customers[customer].demand;

            /* Verifica se a troca é viável */
            if (capacities_free[nova_rota] + nova_carga >= load && capacities_free[rota_original] + load >= nova_carga) {
                fim_rota = individual->routes_end[rota_original];

                /* Calculando o cost da nova customer na posicao da customer antiga. */
                if (fim_rota == 1) {
                    cost_new = calculate_swap_cost_exclusive(original_cost, distances, 0, 0, customer, nova_cidade); /* Macro */
                } else {
                    route = individual->routes[rota_original];
                    get_customer_preceding_and_successor(componente, route, fim_rota, cidade_anterior, cidade_posterior);
                    cost_new = calculate_swap_cost_exclusive(original_cost, distances, cidade_anterior, cidade_posterior, customer, nova_cidade); /* Macro */
                }

                /* Calculando o cost da customer antiga na posicao da nova customer. */
                fim_rota = individual->routes_end[nova_rota];
                route = individual->routes[nova_rota];
                if (fim_rota == 1) {
                    cost_new = calculate_swap_cost_exclusive(cost_new, distances, 0, 0, nova_cidade, customer); /* Macro */
                } else {
                    route[fim_rota] = 0; /* Apenas para modularidade, veja que é utilizado route[1] para determinação. Se a route estiver vazia o caso se torna o mesmo caso não esteja.*/

                    get_customer_preceding_and_successor(nova_componente, route, fim_rota, cidade_anterior, cidade_posterior); /* Macro */
                    cost_new = calculate_swap_cost_exclusive(cost_new, distances, cidade_anterior, cidade_posterior, nova_cidade, customer); /* Macro */
                }

                if (cost_new < original_cost) {
                    individual_swap_customers(individual, customer, load, nova_cidade, nova_carga);
                    original_cost = cost_new;
                    capacities_free[nova_rota] = capacities_free[nova_rota] + nova_carga - load;
                    capacities_free[rota_original] = capacities_free[rota_original] + load - nova_carga;

                    houve_melhora = 1;
                }
            }

            customer++;
        }while (customer < customers_num);

        if (houve_melhora) {
            it_sem_melhora = 0;
        } else  it_sem_melhora++;

    }while (it_sem_melhora < IT_SEM_MELHORA_STP);

    if (!individual->feasible) {
        i = 0;
        while (i < vehicles_num && capacities_free[i] >=0) i++;

        if (i == vehicles_num) {
            individual->cost = original_cost - PENALTY;
            individual->feasible = 1;
            return;
        }
    }

    individual->cost = original_cost;
    return;
}

/*
 * Retorno: 0 se todas as customers possíveis não melhoraram a solucao
 * -1 se n obteve customer possivel
 * o novo valor do individual caso obteve solucao
 */
int drop_one_point_infeasible(Individual* individual, int** distances, Customer* customers, int vehicles_num, int customers_num, int capacidade_max) {  
    int infeasible = 1;

    /* Obtendo a route de maior load */
    int i,
        rota_maior_carga = 0,
        maior_carga_disponivel = 0;
    for (i = 0; i < vehicles_num; i++) {
        if (individual->capacities_free[i] > maior_carga_disponivel) {
            maior_carga_disponivel = individual->capacities_free[i];
            rota_maior_carga = i;
        }
    }

    /* Identificando a route que estourou a capacidade. */
    int rota_invalida = 0;
    while (individual->capacities_free[rota_invalida] >= 0) rota_invalida++;

    int *route = NULL,
        *routes_end = individual->routes_end;
    
    int load,
        customer,
        fim_rota,
        rota_selecionada,
        tentativas,
        max_tentativas;
    do {    
        /* Selecionando uma route inválida aleatória */
        do {
            rota_invalida = rand() % vehicles_num;
        }while (individual->capacities_free[rota_invalida] >= 0);
        
        route = individual->routes[rota_invalida];
        fim_rota = routes_end[rota_invalida];
        
        /* Tentativa de selecionar uma customer aleatória na route que estourou a capacidade */
        tentativas = 0;
        max_tentativas = fim_rota + fim_rota/2;
        do {
            if (tentativas == max_tentativas) 
                return -1; /* Algoritmo chegou ao máximo de tentativas e não encontrou uma customer que pudesse ser realocada naquela route */
        
            customer = route[rand() % fim_rota];
            load = customers[customer].demand;
            
            tentativas++;
        }while (maior_carga_disponivel < load);
        
        
        /* Selecionando uma route aleatória para realocar a customer selecionada */
        do {
            rota_selecionada = rand() % vehicles_num;
        }while (individual->capacities_free[rota_selecionada] < load);

        reinsert_customer_best_position_in_another_route(individual, distances, customer, load, rota_selecionada);

//      individual->capacities_free[num_rota] -= load;
//      individual->capacities_free[rota_invalida] += load;

        //Se a route selecionada foi a route de maior load disponivel, a maior load disponivel será recalculada.
        if (rota_selecionada == rota_maior_carga) {
            maior_carga_disponivel -= load;
            for (i = 0; i < vehicles_num; i++) {
                if (maior_carga_disponivel < individual->capacities_free[i]) {
                    maior_carga_disponivel = individual->capacities_free[i];
                    rota_maior_carga = i;
                }
            }
        }
        
        /* Será verificado se o individual viabilizou */
        if (individual->capacities_free[rota_invalida] >= 0) {
            for (i = 0; i < vehicles_num; i++) {
                if (individual->capacities_free[i] < 0) {
                    break;
                }
            }
            
            if (i == vehicles_num) infeasible = 0;
        }
        
    }while (infeasible);

    individual->feasible = 1;
    return individual->cost = individual->cost - PENALTY;
}

/*
 * Retorno: 0 se todas as customers possíveis não melhoraram a solucao
 * -1 se n obteve customer possivel
 * o novo valor do individual caso obteve solucao
 */
void drop_one_point(Individual* individual, int** distances, Customer* customers, int vehicles_num, int customers_num, int capacidade_max) {
    int i,
        fim_rota,
        load,
        capacidade_disponivel,
        rota_maior_carga = 0;

    int *route = NULL;

    int capacities_free[vehicles_num],
        cidades_possiveis[customers_num],
        rotas_selecionadas[vehicles_num];

    int num_rota = 0,
        maior_carga_disponivel = 0;

    //Calculando a capacidade disponível de cada route.
    do {
        fim_rota = individual->routes_end[num_rota];
        route = individual->routes[num_rota];

        load = 0;
        for (i = 0; i < fim_rota; i++) {
            load = load + customers[ route[i] ].demand;
        }

        capacidade_disponivel = capacidade_max - load;
        if (capacidade_disponivel > maior_carga_disponivel) {
            maior_carga_disponivel = capacidade_disponivel;
            rota_maior_carga = num_rota;
        }

        capacities_free[num_rota] = capacidade_disponivel;
        num_rota++;
    } while (num_rota < vehicles_num);

    int segunda_maior_carga_disponivel = 0,
        segunda_rota_maior_carga = 0,
        primeira_melhor = 1;

    /* Calculando a primeira e segunda route de maior load. */
    for (i = 0; i < vehicles_num; i++) {
        load = capacities_free[i];
        if (load > segunda_maior_carga_disponivel) {
            if (load != maior_carga_disponivel) {
                segunda_maior_carga_disponivel = load;
                segunda_rota_maior_carga = i;
            } else if (primeira_melhor) {
                primeira_melhor = 0;
            } else {
                segunda_maior_carga_disponivel = load;
                segunda_rota_maior_carga = i;
            }
        }
    }

    int customer = 0,
        max_index_cidades = 0;

    for (num_rota = 0; num_rota < vehicles_num; num_rota++) {
        fim_rota = individual->routes_end[num_rota];
        route = individual->routes[num_rota];

        if (num_rota != rota_maior_carga) {
            for (i = 0; i < fim_rota; i++) {
                customer = route[i];    
                if (maior_carga_disponivel >= customers[customer].demand) {
                    cidades_possiveis[max_index_cidades] = customer;
                    max_index_cidades++;
                }
            }
        } else {
            for (i = 0; i < fim_rota; i++) {
                customer = route[i];    
                if (segunda_maior_carga_disponivel >= customers[customer].demand) {
                    cidades_possiveis[max_index_cidades] = customer;
                    max_index_cidades++;
                }
            }
        }
    }

    if (max_index_cidades == 0) return;

    int* posicoes_rotas = individual->positions[0];

    int index,
        got_improvement,
        max_index_rotas,
        rota_selecionada,
        cidade_perturbada,
        perturbacoes = 0,
        num_perturbacoes = 0.2 * customers_num;

    int cidades_fechadas[customers_num];
    memset(cidades_fechadas, 0, customers_num*sizeof(int));

    do {    
        i = 0;
        do {
            cidade_perturbada = cidades_possiveis[rand() % max_index_cidades];
            i++;
        } while (cidades_fechadas[cidade_perturbada] && i < max_index_cidades);

        if (i == max_index_cidades) return;

        cidades_fechadas[cidade_perturbada] = 1;
        num_rota = posicoes_rotas[cidade_perturbada];
        load = customers[cidade_perturbada].demand;

        max_index_rotas = 0;
        for (i = 0; i < vehicles_num; i++) {
            if (capacities_free[i] >= load && i != num_rota) {
                rotas_selecionadas[max_index_rotas] = i;
                max_index_rotas++;
            }
        }

        if (max_index_rotas == 0) return;

        num_rota = rotas_selecionadas[rand() % max_index_rotas]; //nova route.
        rota_selecionada = posicoes_rotas[cidade_perturbada];
        got_improvement = reinsert_customer_best_position_in_another_route_if_improves(individual, distances, cidade_perturbada, load, num_rota);

        if (got_improvement) {
            capacities_free[num_rota] -= load;
            capacities_free[rota_selecionada] += load;

            /* Se a route selecionada foi a route de maior load disponivel, a maior load disponivel será recalculada. */
            if (num_rota == rota_maior_carga) {
                maior_carga_disponivel -= load;
                for (i = 0; i < vehicles_num; i++) {
                    if (maior_carga_disponivel < capacities_free[i]) {
                        maior_carga_disponivel = capacities_free[i];
                        rota_maior_carga = i;
                    }
                }
            }
            //A segunda maior load disponivel também será recalculada caso seja necessário.
            if (num_rota == segunda_rota_maior_carga || segunda_rota_maior_carga >= maior_carga_disponivel) {
                primeira_melhor = 1;
                for (i = 0; i < vehicles_num; i++) {
                    load = capacities_free[i];
                    if (load > segunda_maior_carga_disponivel) {
                        if (load != maior_carga_disponivel) {
                            segunda_maior_carga_disponivel = load;
                            segunda_rota_maior_carga = i;
                        } else if (primeira_melhor) {
                            primeira_melhor = 0;
                        } else {
                            segunda_maior_carga_disponivel = load;
                            segunda_rota_maior_carga = i;
                        }
                    }
                }
            }

            /* Removendo as customers que não possuem mais alguma route que a suporte, diferente da route na qual a customer reside. */
            i = 0;
            do {
                customer = cidades_possiveis[i];
                i++;
            }while (i < max_index_cidades && customer != cidade_perturbada && customers[customer].demand <= maior_carga_disponivel);

            /* Caso encontre ao menos uma customer a ser removida, os elementos da route serão deslocados. */
            if (i < max_index_cidades) {
                index = i -1;
                while (i < max_index_cidades) {
                    customer = cidades_possiveis[i];

                    num_rota = posicoes_rotas[customer];
                    if (num_rota != rota_maior_carga) {
                        if (customer != cidade_perturbada && customers[customer].demand <= maior_carga_disponivel) {
                            cidades_possiveis[index] = cidades_possiveis[i];
                            index++;
                        }
                    } else if (customer != cidade_perturbada && customers[customer].demand <= segunda_maior_carga_disponivel) {
                        cidades_possiveis[index] = cidades_possiveis[i];
                        index++;
                    }

                    i++;
                }
                max_index_cidades = index;

            } else max_index_cidades--;

            if (max_index_cidades == 0) return;
        }

        perturbacoes++;
    }while (perturbacoes < num_perturbacoes);

    return;
}

void flip(Individual* individual, int** distances, Customer* customers, int vehicles_num, int customers_num) {
    int index,
        num_rota,
        fim_rota,
        customer,
        componente,
        custo_base,
        cost_new,
        nova_posicao = 0,
        cidade_anterior,
        cidade_posterior,
        got_improvement,
        houve_melhora_mov,
        original_cost = individual->cost;

    int *route = NULL;

    int it_sem_melhora = 0;
    do {
        num_rota = 0;
        houve_melhora_mov = 0;
        do {
            got_improvement = 0;
            fim_rota = individual->routes_end[num_rota];
            if (fim_rota < 2) {
                if (num_rota +1 < vehicles_num) {
                    num_rota++;
                    fim_rota = individual->routes_end[num_rota];
                } else goto PROX_ITERACAO_FLIP;
            }

            route = individual->routes[num_rota];
            componente = rand() % fim_rota;
            customer = route[componente];

            get_customer_preceding_and_successor_dec_route_end(componente, route, fim_rota, cidade_anterior, cidade_posterior); /* Macro */
            custo_base = calculate_customer_remotion(original_cost, distances, cidade_anterior, cidade_posterior, customer); /* Macro */

            route[ individual->routes_end[num_rota] ] = 0;
            fim_rota++;

            index = 0;
            if (componente != 0) {
                cidade_anterior = 0;
                cidade_posterior = route[0];
            } else {
                index = 2;
                cidade_anterior = cidade_posterior;
                cidade_posterior = route[2];
            }

            while (index < fim_rota) {
                cost_new = calculate_customer_insertion(custo_base, distances, cidade_anterior, cidade_posterior, customer); /* Macro */

                if (cost_new < original_cost) {
                    original_cost = cost_new;
                    nova_posicao = index;       
                    got_improvement = houve_melhora_mov = 1;
                }

                index++;
                if (index == componente) index++;

                cidade_anterior = cidade_posterior;
                cidade_posterior = route[index];
            }

            if (got_improvement) {
                individual_reinsert_customer_in_route(individual, customer, nova_posicao);
                individual->cost = original_cost;
            }

            num_rota++;
        }while (num_rota < vehicles_num);

PROX_ITERACAO_FLIP:
        if (houve_melhora_mov) {
            it_sem_melhora = 0;
        } else  it_sem_melhora++;

    }while (it_sem_melhora < IT_SEM_MELHORA_FLIP);

    return;
}

/* Retorno:
 *   - 0 se não houve melhora.
 *   - 1 se houve melhora.
 */
int reinsert_customer_best_position_in_another_route_if_improves(Individual* individual, int** distances, int customer, int load, int new_route_idx) {
    int cidade_anterior,
        cidade_posterior,
        index_rota = individual->positions[0][customer],
        fim_rota = individual->routes_end[index_rota];

    int* route = individual->routes[index_rota];  
    int componente = individual->positions[1][customer];

    get_customer_preceding_and_successor_dec_route_end(componente, route, fim_rota, cidade_anterior, cidade_posterior); /* Macro */
//  printf("%d %d %d %d\n", individual->cost, cidade_anterior, cidade_posterior, customer);
    int custo_base = calculate_customer_remotion(individual->cost, distances, cidade_anterior, cidade_posterior, customer); /* Macro */

    //  individual_remove_customer(individual, customer);

    int cost_new = 0;
    if (individual->routes_end[new_route_idx] == 0) {
        cost_new = custo_base + distances[0][customer]
                    + distances[customer][0];

        if (cost_new < individual->cost) {
            individual_remove_customer(individual, customer, load);
            individual_insert_customer(individual, customer, load, 0, new_route_idx);
            individual->cost = cost_new;
            return 1;
        }
        return 0;
    }

    route = individual->routes[new_route_idx]; 
    cidade_anterior = 0;
    cidade_posterior = route[0];
    fim_rota = individual->routes_end[new_route_idx];

    int index = 0,
        original_cost = INT_MAX,
        nova_posicao = 0;

    route[fim_rota] = 0;
    fim_rota++;
    while (index < fim_rota) {
        cost_new = calculate_customer_insertion(custo_base, distances, cidade_anterior, cidade_posterior, customer); /* Macro */

        if (cost_new < original_cost) {
            original_cost = cost_new;
            nova_posicao = index;
        }

        index++;
        cidade_anterior = cidade_posterior;
        cidade_posterior = route[index];
    }

    if (original_cost < individual->cost) {
        individual_remove_customer(individual, customer, load);
        individual_insert_customer(individual, customer, load, nova_posicao, new_route_idx);
        individual->cost = original_cost;
        return 1;
    }
    return 0;
}

void reinsert_customer_best_position_in_another_route(Individual* individual, int** distances, int customer, int load, int new_route_idx) {
    int cidade_anterior,
        cidade_posterior,
        index_rota = individual->positions[0][customer],
        fim_rota = individual->routes_end[index_rota];

    int* route = individual->routes[index_rota];
    int componente = individual->positions[1][customer];

    get_customer_preceding_and_successor_dec_route_end(componente, route, fim_rota, cidade_anterior, cidade_posterior); /* Macro */
    int custo_base = calculate_customer_remotion(individual->cost, distances, cidade_anterior, cidade_posterior, customer); /* Macro */

    individual_remove_customer(individual, customer, load);

    if (individual->routes_end[new_route_idx] == 0) {
        individual_insert_customer(individual, customer, load, 0, new_route_idx);
        individual->cost = custo_base + distances[0][customer]
            + distances[customer][0];
        return;
    }

    route = individual->routes[new_route_idx]; 
    cidade_anterior = 0;
    cidade_posterior = route[0];

    int cost_new,
        nova_posicao = 0,
        original_cost = INT_MAX;

    fim_rota = individual->routes_end[new_route_idx];
    route[fim_rota] = 0;
    fim_rota++;

    int index = 0;
    while (index < fim_rota) {
        cost_new = calculate_customer_insertion(custo_base, distances, cidade_anterior, cidade_posterior, customer); /* Macro */

        if (cost_new < original_cost) {
            original_cost = cost_new;
            nova_posicao = index;
        }

        index++;
        cidade_anterior = cidade_posterior;
        cidade_posterior = route[index];
    }

    individual_insert_customer(individual, customer, load, nova_posicao, new_route_idx);
    individual->cost = original_cost;
    return;
}


void strong_drop_one_point(Individual* individual, int** distances, Customer* customers, int vehicles_num, int customers_num, int capacidade_max) {
    int rotas_possiveis[vehicles_num];

    int i, k, l,
        load,
        customer,
        fim_rota,
        num_rota,
        rota_selecionada;

    int got_improvement = 0,
        load_old = -1; //TODO: use load, new load instead?
    
    int* route = NULL;
    
    int index = 0;
    for (num_rota = 0; num_rota < vehicles_num; num_rota++) {
    
        route = individual->routes[num_rota];
        fim_rota = individual->routes_end[num_rota];
        load_old = -1;
        
        /*Se a route só possui uma customer, esta customer será mantida */
        if (fim_rota > 1) { 
            for (i = 0; i < fim_rota; i++) {
                customer = route[i];
                load = customers[customer].demand;
                
                /* Se a load atual é maior que a load anterior ou houve melhora, é necessário recalcular as routes possiveis */
                if (load > load_old || got_improvement) {
                    //Seleciona as routes possiveis, diferentes da route atual, para efetuar o movimento.
                    index = 0;
                    for (k = 0; k < vehicles_num; k++) {
                        if (k != num_rota && load <= individual->capacities_free[k]) {
                            rotas_possiveis[index] = k;
                            index++;
                        }
                    }
                }
                
                got_improvement = 0;
                for (l = 0; l < index; l++) {
                    rota_selecionada = rotas_possiveis[l];
                    if (!got_improvement) {
                        got_improvement = reinsert_customer_best_position_in_another_route_if_improves(individual, distances, customer, load, rota_selecionada);
                    } else {
                        reinsert_customer_best_position_in_another_route_if_improves(individual, distances, customer, load, rota_selecionada);
                    }
                }
                    
                /* Com a melhora, o tamanho das routes modificaram */
                if (got_improvement)
                    fim_rota = individual->routes_end[num_rota];
                
                load_old = load;
            }
        }
    }

    if (!individual->feasible) {
        i = 0;
        while (i < vehicles_num && individual->capacities_free[i] >=0) i++;

        if (i == vehicles_num) {
            individual->cost -= PENALTY;
            individual->feasible = 1;
        }
    }
    return;
}





