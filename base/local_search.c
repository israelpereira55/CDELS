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
    int i = 0, 
        j = 0,
        routei = 0,
        routej = 0,
        load = 0, 
        loadj = 0,
        customeri = 0, 
        customerj = 0,
        route_endi = 0, 
        route_endj = 0,
        icustomer_preceding = 0, 
        icustomer_successor = 0,
        jcustomer_preceding = 0, 
        jcustomer_successor = 0;
     
    int *route = NULL;
    
    int cost_new = 0,
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
        while (i < vehicles_num && individual->capacities_free[i] >=0) {
            i++;
        }

        if (i == vehicles_num) {
            individual->cost = original_cost - PENALTY;
            individual->feasible = 1;
            return;
        }
    }
    
    individual->cost = original_cost;
    return;
}


int drop_one_point_infeasible(Individual* individual, int** distances, Customer* customers, int vehicles_num, int customers_num, int capacidade_max) {  
    int infeasible = 1;

    /* Obtaining route with highest load */
    int i = 0,
        route_highest_load = 0,
        highest_free_capacity = 0;

    for (i = 0; i < vehicles_num; i++) {
        if (individual->capacities_free[i] > highest_free_capacity) {
            highest_free_capacity = individual->capacities_free[i];
            route_highest_load = i;
        }
    }

    int *route = NULL;
    
    int load = 0,
        customer = 0,
        route_end = 0,
        retry_cnt = 0,
        retry_max = 0,
        route_chosen = 0,
        route_invalid = 0;
    do {    
        /* Selecionando uma route inválida aleatória */
        do {
            route_invalid = rand() % vehicles_num;
        }while (individual->capacities_free[route_invalid] >= 0); //TODO: make a list of infeasible routes
        
        route = individual->routes[route_invalid];
        route_end = individual->routes_end[route_invalid];
        
        /* Tentativa de selecionar uma customer aleatória na route que estourou a capacidade */
        retry_cnt = 0;
        retry_max = route_end + route_end/2;
        do {
            if (retry_cnt == retry_max) 
                return -1; /* Algoritmo chegou ao máximo de retry_cnt e não encontrou uma customer que pudesse ser realocada naquela route */
        
            customer = route[rand() % route_end];
            load = customers[customer].demand;
            
            retry_cnt++;
        }while (highest_free_capacity < load);
        
        
        /* Selecionando uma route aleatória para realocar a customer selecionada */
        do {
            route_chosen = rand() % vehicles_num;
        }while (individual->capacities_free[route_chosen] < load);

        reinsert_customer_best_position_in_another_route(individual, distances, customer, load, route_chosen);

//      individual->capacities_free[num_rota] -= load;
//      individual->capacities_free[route_invalid] += load;

        //Se a route selecionada foi a route de maior load disponivel, a maior load disponivel será recalculada.
        if (route_chosen == route_highest_load) {
            highest_free_capacity -= load;
            for (i = 0; i < vehicles_num; i++) {
                if (highest_free_capacity < individual->capacities_free[i]) {
                    highest_free_capacity = individual->capacities_free[i];
                    route_highest_load = i;
                }
            }
        }
        
        /* Será verificado se o individual viabilizou */
        if (individual->capacities_free[route_invalid] >= 0) {
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


//TODO: can merge it with the other function?
int reinsert_customer_best_position_in_another_route_if_improves(Individual* individual, int** distances, int customer, int load, int new_route_idx) {
    int customer_preceding = 0,
        customer_successor = 0,
        index_rota = individual->positions[0][customer],
        route_end = individual->routes_end[index_rota];

    int* route = individual->routes[index_rota];  
    int component = individual->positions[1][customer];

    get_customer_preceding_and_successor_dec_route_end(component, route, route_end, customer_preceding, customer_successor); /* Macro */
//  printf("%d %d %d %d\n", individual->cost, customer_preceding, customer_successor, customer);
    int custo_base = calculate_customer_remotion(individual->cost, distances, customer_preceding, customer_successor, customer); /* Macro */

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
    customer_preceding = 0;
    customer_successor = route[0];
    route_end = individual->routes_end[new_route_idx];

    int index = 0,
        original_cost = INT_MAX,
        nova_posicao = 0;

    route[route_end] = 0;
    route_end++;
    while (index < route_end) {
        cost_new = calculate_customer_insertion(custo_base, distances, customer_preceding, customer_successor, customer); /* Macro */

        if (cost_new < original_cost) {
            original_cost = cost_new;
            nova_posicao = index;
        }

        index++;
        customer_preceding = customer_successor;
        customer_successor = route[index];
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
    int customer_preceding = 0,
        customer_successor = 0,
        index_rota = individual->positions[0][customer],
        route_end = individual->routes_end[index_rota];

    int* route = individual->routes[index_rota];
    int component = individual->positions[1][customer];

    get_customer_preceding_and_successor_dec_route_end(component, route, route_end, customer_preceding, customer_successor); /* Macro */
    int custo_base = calculate_customer_remotion(individual->cost, distances, customer_preceding, customer_successor, customer); /* Macro */

    individual_remove_customer(individual, customer, load);

    if (individual->routes_end[new_route_idx] == 0) {
        individual_insert_customer(individual, customer, load, 0, new_route_idx);
        individual->cost = custo_base + distances[0][customer]
            + distances[customer][0];
        return;
    }

    route = individual->routes[new_route_idx]; 
    customer_preceding = 0;
    customer_successor = route[0];

    int cost_new,
        nova_posicao = 0,
        original_cost = INT_MAX;

    route_end = individual->routes_end[new_route_idx];
    route[route_end] = 0;
    route_end++;

    int index = 0;
    while (index < route_end) {
        cost_new = calculate_customer_insertion(custo_base, distances, customer_preceding, customer_successor, customer); /* Macro */

        if (cost_new < original_cost) {
            original_cost = cost_new;
            nova_posicao = index;
        }

        index++;
        customer_preceding = customer_successor;
        customer_successor = route[index];
    }

    individual_insert_customer(individual, customer, load, nova_posicao, new_route_idx);
    individual->cost = original_cost;
    return;
}


void strong_drop_one_point(Individual* individual, int** distances, Customer* customers, int vehicles_num, int customers_num, int capacidade_max) {
    int rotas_possiveis[vehicles_num];

    int i = 0, 
        load = 0,
        customer = 0,
        route_end = 0,
        route_chosen = 0;

    int load_old = 0,    //TODO: use load, new load instead?
        got_improvement = 0;
    
    int* route = NULL;
    
    int index = 0;
    for (int num_rota = 0; num_rota < vehicles_num; num_rota++) {
    
        route = individual->routes[num_rota];
        route_end = individual->routes_end[num_rota];
        load_old = -1;
        
        /*Se a route só possui uma customer, esta customer será mantida */
        if (route_end > 1) { 
            for (i = 0; i < route_end; i++) {
                customer = route[i];
                load = customers[customer].demand;
                
                /* Se a load atual é maior que a load anterior ou houve melhora, é necessário recalcular as routes possiveis */
                if (load > load_old || got_improvement) {
                    //Seleciona as routes possiveis, diferentes da route atual, para efetuar o movimento.
                    index = 0;
                    for (int k = 0; k < vehicles_num; k++) {
                        if (k != num_rota && load <= individual->capacities_free[k]) {
                            rotas_possiveis[index] = k;
                            index++;
                        }
                    }
                }
                
                got_improvement = 0;
                for (int l = 0; l < index; l++) {
                    route_chosen = rotas_possiveis[l];
                    if (!got_improvement) {
                        got_improvement = reinsert_customer_best_position_in_another_route_if_improves(individual, distances, customer, load, route_chosen);
                    } else {
                        reinsert_customer_best_position_in_another_route_if_improves(individual, distances, customer, load, route_chosen);
                    }
                }
                    
                /* Com a melhora, o tamanho das routes modificaram */
                if (got_improvement)
                    route_end = individual->routes_end[num_rota];
                
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
