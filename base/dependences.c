#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../metaheuristic/differential_evolution.h"


Customer* customer_create(int id, double x, double y) {
	Customer* customer = (Customer*) malloc(sizeof(Customer));
	customer->id = id;
	customer->x = x;
	customer->y = y;
	customer->demand = 0;
	return customer;
}


Customer* customer_free(Customer* customer) {
	free(customer);
	return NULL;
}


/* Alternative for distance matrix */
/*
int calculate_distance(Customer* c1, Customer* c2){
	return sqrt( pow( (c1->x - c2->x), 2) + pow( (c1->y - c2->y), 2) ) + 0.5;
}
*/


int** distances_matrix_init(Customer* customers, int customers_num) {
	int i, j, cost;
	    
	int** mat = (int**) malloc (customers_num * sizeof(int*));
	for(i = 0; i < customers_num; i++)
		mat[i] = (int*) malloc (customers_num * sizeof(int));
	
	Customer *c1, *c2;
	for(i = 1; i < customers_num; i++){
		c1 = &customers[i];
		for(j = i -1; j >= 0; j--){
			c2 = &customers[j];
			cost = sqrt( pow((c1->x - c2->x), 2) + pow((c1->y - c2->y), 2) ) + 0.5;
			mat[i][j] = mat[j][i] = cost;
		}
	}
	
	for(i = 0; i < customers_num; i++)
		mat[i][i] = 0;
		
	return mat;
}


int** distances_matrix_free(int** distances, int customers_num) {
	int i;
	for(i = 0; i < customers_num; i++)
		free(distances[i]);
	
	free(distances);
	return NULL;
}

		
Individual* individual_init(int customers_num, int vehicles_num) {
	Individual* individual = (Individual*) malloc(sizeof(Individual));
	individual->routes = (int**) malloc(vehicles_num * sizeof(int*));
	
	int i;
	for(i = 0; i < vehicles_num; i++)
		individual->routes[i] = (int*) malloc(customers_num * sizeof(int));

	individual->positions =   (int**) malloc(2 * sizeof(int*));
	individual->positions[0] = (int*) malloc(customers_num * sizeof(int));
	individual->positions[1] = (int*) malloc(customers_num * sizeof(int));

	individual->positions[0][0] = individual->positions[1][0] = 0;

	individual->routes_end = (int*) malloc (vehicles_num * sizeof(int));
	individual->capacities_free = (int*) malloc (vehicles_num * sizeof(int));
	
	individual->cloned = 0;
	return individual;
}

Individual* individual_free(Individual* individual, int vehicles_num) {
	int i;
	for(i = 0; i < vehicles_num; i++)
		free(individual->routes[i]);
		
	free(individual->routes);
	
	free(individual->capacities_free);
	free(individual->positions[0]);
	free(individual->positions[1]);
	free(individual->positions);
	
	free(individual->routes_end);
	free(individual);
	return NULL;
}

Individual* individual_generate_top_to_down(int** distances, Customer* customers, int customers_num, int capacity_max, int vehicles_num) {
    int load,
        random,
        route_load;
        
    int customers_checked[customers_num],
        customers_routed[customers_num];
        
    memset(customers_checked, 0, customers_num*sizeof(int));
    memset(customers_routed, 0,  customers_num*sizeof(int));
    customers_checked[0] = 1;
    customers_routed[0] = 1;
    
    Individual* individual = individual_init(customers_num, vehicles_num);
    
    int index = 0,
        customers_checked_cnt = 1,
        customers_routed_cnt = 1;
    
    int routes_planned_cnt;
    for(routes_planned_cnt = 0; routes_planned_cnt < vehicles_num; routes_planned_cnt++){

        route_load = 0;
        do{
            do{
                random = (rand() % (customers_num-1)) +1; //+1 pois 0 é o posto; raio 1-31.
            }while(customers_checked[random]);

            load = customers[random].demand;
            route_load += load;

            if(route_load < capacity_max) {
                individual->routes[routes_planned_cnt][index] = random;
                individual->positions[0][random] = routes_planned_cnt;
                individual->positions[1][random] = index;

                customers_routed[random] = 1;
                customers_routed_cnt++;
                index++;
                
            } else route_load = route_load - load;
        
            customers_checked[random] = 1;
            customers_checked_cnt++;
            
        } while(customers_checked_cnt < customers_num);
        
        customers_checked_cnt = 1;
        for(int i = 1; i < customers_num; i++){
            if(customers_routed[i]){
                customers_checked[i] = 1;
                customers_checked_cnt++;
            } else customers_checked[i] = 0;
        }

        individual->routes_end[routes_planned_cnt] = index;
        index = 0;
    }
    
    //Verificando viabilidade do individual.
    if(customers_routed_cnt == customers_num){
        individual_reevaluate(individual, capacity_max, vehicles_num, distances, customers);
        return individual;
    }
    
    /* Caso ainda existam customers que não foram alocadas em alguma route, elas serão inseridas na ultima route */
    routes_planned_cnt--;
    index = individual->routes_end[routes_planned_cnt];
    
    for(int i = 1; i < customers_num; i++){
        if(!customers_routed[i]){
            individual->routes[routes_planned_cnt][index] = i;
            individual->positions[0][i] = routes_planned_cnt;
            individual->positions[1][i] = index;
            
            index++;
        }
    }
    
    individual->routes_end[routes_planned_cnt] = index;
    individual_reevaluate(individual, capacity_max, vehicles_num, distances, customers);
    return individual;
}

//TODO: make top to down and down to top in the same function
Individual* individual_gererate_down_to_top(int** distances, Customer* customers, int customers_num, int capacity_max, int vehicles_num) {
    int load,
        random,
        route_load;
        
    int customers_checked[customers_num],
        customers_routed[customers_num];
        
    memset(customers_checked, 0, customers_num*sizeof(int));
    memset(customers_routed, 0,  customers_num*sizeof(int));
    customers_checked[0] = 1;
    customers_routed[0] = 1;
    
    Individual* individual = individual_init(customers_num, vehicles_num);
    
    int index = 0,
        customers_checked_cnt = 1,
        customers_routed_cnt = 1;
    
    int routes_planned_cnt;
    for(routes_planned_cnt = vehicles_num -1; routes_planned_cnt >= 0; routes_planned_cnt--){
    
        route_load = 0;
        do {
            do{
                random = (rand() % (customers_num-1)) +1; //+1 pois 0 é o posto; raio 1-31.
            }while(customers_checked[random]);

            load = customers[random].demand;
            route_load = route_load + load;

            if(route_load < capacity_max) {
                individual->routes[routes_planned_cnt][index] = random;
                individual->positions[0][random] = routes_planned_cnt;
                individual->positions[1][random] = index;

                customers_routed[random] = 1;
                customers_routed_cnt++;
                index++;

            } else route_load = route_load - load;
        
            customers_checked[random] = 1;
            customers_checked_cnt++;
        } while(customers_checked_cnt < customers_num);
        
        customers_checked_cnt = 1;
        for(int i = 1; i < customers_num; i++){
            if(customers_routed[i]){
                customers_checked[i] = 1;
                customers_checked_cnt++;
                
            } else  customers_checked[i] = 0;
        }

        individual->routes_end[routes_planned_cnt] = index;
        index = 0;
    }
    
    //Verificando viabilidade do individual.
    if(customers_routed_cnt == customers_num){
        individual_reevaluate(individual, capacity_max, vehicles_num, distances, customers);
        return individual;
    }
    
    /* Caso ainda existam customers que não foram alocadas em alguma route, elas serão inseridas na primeira route */
    routes_planned_cnt++;
    index = individual->routes_end[routes_planned_cnt];
    
    for(int i = 1; i < customers_num; i++){
        if(!customers_routed[i]){
            individual->routes[routes_planned_cnt][index] = i;
            individual->positions[0][i] = routes_planned_cnt;
            individual->positions[1][i] = index;
            
            index++;
        }
    }
    
    individual->routes_end[routes_planned_cnt] = index; 
    individual_reevaluate(individual, capacity_max, vehicles_num, distances, customers);
    return individual;
}
		
void individual_update_attributes(Individual* individual, int capacity_max, int vehicles_num, int** distances, Customer* customers) {
	int *route,
	    *routes_end = individual->routes_end,
	    *capacities_free = individual->capacities_free;
	     
	int cost,
	    route_load,
	    component,
	    customer_current,
	    customer_before;
	    
	individual->feasible = 1;
	individual->cost = 0;
		
	int route_end;
	for(int i = 0; i < vehicles_num; i++) {
	
		route = individual->routes[i];
		route_end = routes_end[i];
		route_load = 0;
		for(int j = 0; j < route_end; j++){
			component = route[j];
			route_load += customers[component].demand;
		}
		capacities_free[i] = capacity_max - route_load;
		
		if(route_load > capacity_max && individual->feasible){
			individual->feasible = 0;
			individual->cost += PENALTY;
		}
		
		//Calculando o cost.
		customer_before = 0;
		cost = 0;
		for(int j = 0; j < route_end; j++){
			customer_current = route[j]; 
			cost += distances[customer_before][customer_current];
			customer_before = customer_current;
		}
		
		cost += distances[customer_before][0];
		individual->cost += cost;
	}
	return;
}

/* Não são clonadas as cargas disponiveis */
Individual* individual_make_hard_clone(Individual* individual, int customers_num, int vehicles_num) {
	Individual* clone = individual_init(customers_num, vehicles_num);
	
	int *route_clone,
	    *route_individual;
	    
	int i, j,
	    route_end = 0;
	for(i = 0; i < vehicles_num; i++) {
		route_clone = clone->routes[i];
		route_individual = individual->routes[i];
		clone->routes_end[i] = individual->routes_end[i];
		
		route_end = individual->routes_end[i];
		for(j = 0; j < route_end; j++)
			route_clone[j] = route_individual[j];
	}
	
	for(j = 0; j < customers_num; j++){
		clone->positions[0][j] = individual->positions[0][j];
		clone->positions[1][j] = individual->positions[1][j];
	}

	clone->cost = individual->cost;
	clone->feasible = individual->feasible;
	return clone;
}

/* Deve ser inserido em outra route */
void individual_insert_customer(Individual* individual, int customer, int load, int new_idx, int new_route) {
	int *route = individual->routes[new_route],
	     position = individual->routes_end[new_route];
	
	int customer_chosen;
	while(position > new_idx){
		customer_chosen = route[position -1];
		route[position] = customer_chosen;
		individual->positions[1][customer_chosen]++;
		position--;
	}
	
	individual->positions[0][customer] = new_route;
	route[position] = customer;
	individual->positions[1][customer] = position;
	individual->routes_end[new_route]++;
	
	individual->capacities_free[new_route] -= load; 
	return;
}

void individual_reinsert_customer_in_route(Individual* individual, int customer, int new_idx) {
	int *route = individual->routes[ individual->positions[0][customer] ],
	     position = individual->positions[1][customer];

	int customer_chosen;
	if(new_idx < position){
		while(position > new_idx){
			customer_chosen = route[position -1];
			route[position] = customer_chosen;
			individual->positions[1][customer_chosen]++;
			position--;
		}
		
		route[position] = customer;
		individual->positions[1][customer] = position;
		return;
	}
	
	new_idx--;
	while(position < new_idx){
		customer_chosen = route[position +1];
		route[position] = customer_chosen;
		individual->positions[1][customer_chosen]--;
		position++;
	}
	
	route[position] = customer;
	individual->positions[1][customer] = position;
	return;
}
/* Sem uso
void individual_insere_cidade_fim_rota(Individual* individual, int customer, int load, int route){
	int index = individual->routes_end[route];
	individual->routes_end[route]++;

	individual->capacities_free[route] -= load;
	individual->routes[route][index] = customer;
	individual->positions[0][customer] = route;
	individual->positions[1][customer] = index;
	return;
}*/

/* Se as customers estiverem na mesma route, passar as cargas como 0 pois não mudarão */
void individual_swap_customers(Individual* individual, int customer1, int load1, int customer2, int load2) {
	int rota1 =    individual->positions[0][customer1],
	    posicao1 = individual->positions[1][customer1],
	    rota2 =    individual->positions[0][customer2],
	    posicao2 = individual->positions[1][customer2];
	
	individual->routes[rota2][posicao2] = customer1;
	individual->routes[rota1][posicao1] = customer2;
	
	individual->positions[0][customer2] = rota1;
	individual->positions[1][customer2] = posicao1;
	individual->capacities_free[rota1] = individual->capacities_free[rota1] + load1 - load2;

	individual->positions[0][customer1] = rota2;
	individual->positions[1][customer1] = posicao2;
	individual->capacities_free[rota2] = individual->capacities_free[rota2] - load1 + load2;

	return;
}

void individual_remove_customer(Individual* individual, int customer, int load) { //TODO: pass customer instead
	int  route_idx = individual->positions[0][customer],
	     route_end = individual->routes_end[route_idx],
	     index = individual->positions[1][customer];
	
	individual->capacities_free[route_idx] += load;
	individual->routes_end[route_idx]--;
	
	//Se a customer a ser removida é a última customer da route.
	if(index +1 == route_end)
		return;
	
	int *route = individual->routes[route_idx],
	     customer_new = 0;
	while(index < route_end -1){
		customer_new =  route[index +1];
		route[index] = customer_new;
		individual->positions[1][customer_new] = index;
		index++;
	}
	return;
}



