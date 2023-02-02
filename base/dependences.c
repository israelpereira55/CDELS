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
	    vehicle_capacity;
	    
	int cidades_visitadas[customers_num],
	    cidades_fechadas[customers_num];
	    
	memset(cidades_visitadas, 0, customers_num*sizeof(int));
	memset(cidades_fechadas, 0,  customers_num*sizeof(int));
	cidades_visitadas[0] = 1;
	cidades_fechadas[0] = 1;
	
	Individual* individual = individuo_inicializa(customers_num, vehicles_num);
	int **routes = individual->routes,
	    *routes_end = individual->routes_end,
	    *posicoes_rotas = individual->positions[0],
	    *posicoes_cidades = individual->positions[1];
	
	int index = 0,
	    num_cidades_visitadas = 1,
	    num_cidades_fechadas = 1;
	
	int i, rotas_fechadas;
	for(rotas_fechadas = 0; rotas_fechadas < vehicles_num; rotas_fechadas++){

		vehicle_capacity = 0;
		do{
			do{
				random = (rand() % (customers_num-1)) +1; //+1 pois 0 é o posto; raio 1-31.
			}while(cidades_visitadas[random]);

			load = customers[random].demand;
			vehicle_capacity += load;

			if(vehicle_capacity < capacidade_max) {
				routes[rotas_fechadas][index] = random;
				posicoes_rotas[random] = rotas_fechadas;
				posicoes_cidades[random] = index;

				cidades_fechadas[random] = 1;
				num_cidades_fechadas++;
				index++;
				
			} else vehicle_capacity = vehicle_capacity - load;
		
			cidades_visitadas[random] = 1;
			num_cidades_visitadas++;
			
		} while(num_cidades_visitadas < customers_num);
		
		num_cidades_visitadas = 1;
		for(i = 1; i < customers_num; i++){
			if(cidades_fechadas[i]){
				cidades_visitadas[i] = 1;
				num_cidades_visitadas++;
			} else cidades_visitadas[i] = 0;
		}

		routes_end[rotas_fechadas] = index;
		index = 0;
	}
	
	//Verificando viabilidade do individual.
	if(num_cidades_fechadas == customers_num){
		individuo_atualiza_atributos(individual, capacidade_max, vehicles_num, distances, customers);
		return individual;
	}
	
	/* Caso ainda existam customers que não foram alocadas em alguma route, elas serão inseridas na ultima route */
	rotas_fechadas--;
	index = routes_end[rotas_fechadas];
	
	for(i = 1; i < customers_num; i++){
		if(!cidades_fechadas[i]){
			routes[rotas_fechadas][index] = i;
			posicoes_rotas[i] = rotas_fechadas;
			posicoes_cidades[i] = index;
			
			index++;
		}
	}
	
	routes_end[rotas_fechadas] = index;
	individuo_atualiza_atributos(individual, capacidade_max, vehicles_num, distances, customers);
	return individual;
}

Individual* individual_gererate_down_to_top(int** distances, Customer* customers, int customers_num, int capacity_max, int vehicles_num) {
	int load,
	    random,
	    vehicle_capacity;
	    
	int cidades_visitadas[customers_num],
	    cidades_fechadas[customers_num];
	    
	memset(cidades_visitadas, 0, customers_num*sizeof(int));
	memset(cidades_fechadas, 0,  customers_num*sizeof(int));
	cidades_visitadas[0] = 1;
	cidades_fechadas[0] = 1;
	
	Individual* individual = individuo_inicializa(customers_num, vehicles_num);
	int **routes = individual->routes,
	    *routes_end = individual->routes_end,
	    *posicoes_rotas = individual->positions[0],
	    *posicoes_cidades = individual->positions[1];
	
	int index = 0,
	    num_cidades_visitadas = 1,
	    num_cidades_fechadas = 1;
	
	int i, rotas_fechadas;
	for(rotas_fechadas = vehicles_num -1; rotas_fechadas >= 0; rotas_fechadas--){
	
		vehicle_capacity = 0;
		do {
			do{
				random = (rand() % (customers_num-1)) +1; //+1 pois 0 é o posto; raio 1-31.
			}while(cidades_visitadas[random]);

			load = customers[random].demand;
			vehicle_capacity = vehicle_capacity + load;

			if(vehicle_capacity < capacidade_max) {
				routes[rotas_fechadas][index] = random;
				posicoes_rotas[random] = rotas_fechadas;
				posicoes_cidades[random] = index;

				cidades_fechadas[random] = 1;
				num_cidades_fechadas++;
				index++;

			} else vehicle_capacity = vehicle_capacity - load;
		
			cidades_visitadas[random] = 1;
			num_cidades_visitadas++;
		} while(num_cidades_visitadas < customers_num);
		
		num_cidades_visitadas = 1;
		for(i = 1; i < customers_num; i++){
			if(cidades_fechadas[i]){
				cidades_visitadas[i] = 1;
				num_cidades_visitadas++;
				
			} else  cidades_visitadas[i] = 0;
		}

		routes_end[rotas_fechadas] = index;
		index = 0;
	}
	
	//Verificando viabilidade do individual.
	if(num_cidades_fechadas == customers_num){
		individuo_atualiza_atributos(individual, capacidade_max, vehicles_num, distances, customers);
		return individual;
	}
	
	/* Caso ainda existam customers que não foram alocadas em alguma route, elas serão inseridas na primeira route */
	rotas_fechadas++;
	index = routes_end[rotas_fechadas];
	
	for(i = 1; i < customers_num; i++){
		if(!cidades_fechadas[i]){
			routes[rotas_fechadas][index] = i;
			posicoes_rotas[i] = rotas_fechadas;
			posicoes_cidades[i] = index;
			
			index++;
		}
	}
	
	routes_end[rotas_fechadas] = index;	
	individuo_atualiza_atributos(individual, capacidade_max, vehicles_num, distances, customers);
	return individual;
}
		
void individual_update_attributes(Individual* individual, int capacity_max, int vehicles_num, int** distances, Customer* customers) {
	int **routes = individual->routes;
	
	int *route,
	    *routes_end = individual->routes_end,
	    *capacities_free = individual->capacities_free;
	     
	int cost,
	    vehicle_capacity,
	    componente,
	    cidade_atual,
	    cidade_anterior;
	    
	individual->viavel = 1;
	individual->cost = 0;
		
	int i, j,
	    fim_rota;
	for(i = 0; i < vehicles_num; i++) {
	
		route = routes[i];
		fim_rota = routes_end[i];
		vehicle_capacity = 0;
		for(j = 0; j < fim_rota; j++){
			componente = route[j];
			vehicle_capacity += customers[componente].demand;
		}
		capacities_free[i] = capacidade_max - vehicle_capacity;
		
		if(vehicle_capacity > capacidade_max && individual->viavel){
			individual->viavel = 0;
			individual->cost += PENALIDADE;
		}
		
		//Calculando o cost.
		cidade_anterior = 0;
		cost = 0;
		for(j = 0; j < fim_rota; j++){
			cidade_atual = route[j]; 
			cost += distances[cidade_anterior][cidade_atual];
			cidade_anterior = cidade_atual;
		}
		
		cost += distances[cidade_anterior][0];
		individual->cost += cost;
	}
	return;
}

/* Não são clonadas as cargas disponiveis */
Individual* individual_make_hard_clone(Individual* individual, int customers_num, int vehicles_num) {
	Individual* clone = individuo_inicializa(customers_num, vehicles_num);
	
	int **rotas_clone = clone->routes,
	    **rotas_individuo = individual->routes;
	
	int *rota_clone,
	    *rota_individuo,
	    *clone_fim_rotas = clone->routes_end,
	    *individuo_fim_rotas = individual->routes_end;
	    
	int *posicoes_clone_rotas = clone->positions[0],
	    *posicoes_clone_cidades = clone->positions[1],
	    *posicoes_individuo_rotas = individual->positions[0],
	    *posicoes_individuo_cidades = individual->positions[1];
	    
	int i, j,
	    fim_rota = 0;
	for(i = 0; i < vehicles_num; i++) {
		rota_clone = rotas_clone[i];
		rota_individuo = rotas_individuo[i];
		clone_fim_rotas[i] = individuo_fim_rotas[i];
		
		fim_rota = individuo_fim_rotas[i];
		for(j = 0; j < fim_rota; j++)
			rota_clone[j] = rota_individuo[j];
	}
	
	for(j = 0; j < customers_num; j++){
		posicoes_clone_rotas[j] = posicoes_individuo_rotas[j];
		posicoes_clone_cidades[j] = posicoes_individuo_cidades[j];
	}

	clone->cost = individual->cost;
	clone->viavel = individual->viavel;
	return clone;
}

/* Deve ser inserido em outra route */
void individual_insert_customer(Individual* individual, int customer, int load, int nex_idx, int new_route) {
	int *posicoes_cidades = individual->positions[1],
	    *route = individual->routes[nova_rota],
	     posicao = individual->routes_end[nova_rota];
	
	int cidade_selecionada;
	while(posicao > nova_posicao){
		cidade_selecionada = route[posicao -1];
		route[posicao] = cidade_selecionada;
		posicoes_cidades[cidade_selecionada]++;
		posicao--;
	}
	
	individual->positions[0][customer] = nova_rota;
	route[posicao] = customer;
	posicoes_cidades[customer] = posicao;
	individual->routes_end[nova_rota]++;
	
	individual->capacities_free[nova_rota] -= load; 
	return;
}

void individual_reinsert_customer_in_route(Individual* individual, int customer, int new_idx) {
	int *posicoes_cidades = individual->positions[1],
	    *route = individual->routes[ individual->positions[0][customer] ],
	     posicao = posicoes_cidades[customer];

	int cidade_selecionada;
	if(novo_index < posicao){
		while(posicao > novo_index){
			cidade_selecionada = route[posicao -1];
			route[posicao] = cidade_selecionada;
			posicoes_cidades[cidade_selecionada]++;
			posicao--;
		}
		
		route[posicao] = customer;
		posicoes_cidades[customer] = posicao;
		return;
	}
	
	novo_index--;
	while(posicao < novo_index){
		cidade_selecionada = route[posicao +1];
		route[posicao] = cidade_selecionada;
		posicoes_cidades[cidade_selecionada]--;
		posicao++;
	}
	
	route[posicao] = customer;
	posicoes_cidades[customer] = posicao;
	return;
}
/* Sem uso
void individuo_insere_cidade_fim_rota(Individual* individual, int customer, int load, int route){
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
	int **routes = individual->routes,
	    *posicoes_rotas = individual->positions[0],
	    *posicoes_cidades = individual->positions[1];
	
	int rota1 = posicoes_rotas[cidade1],
	    posicao1 = posicoes_cidades[cidade1],
	    rota2 = posicoes_rotas[cidade2],
	    posicao2 = posicoes_cidades[cidade2];
	
	routes[rota2][posicao2] = cidade1;
	routes[rota1][posicao1] = cidade2;
	
	posicoes_rotas[cidade2] = rota1;
	posicoes_cidades[cidade2] = posicao1;
	individual->capacities_free[rota1] = individual->capacities_free[rota1] + carga1 - carga2;

	posicoes_rotas[cidade1] = rota2;
	posicoes_cidades[cidade1] = posicao2;
	individual->capacities_free[rota2] = individual->capacities_free[rota2] - carga1 + carga2;

	return;
}

void individual_remove_customer(Individual* individual, int customer, int load) { //TODO: pass customer instead
	int *posicoes_cidades = individual->positions[1],
	     num_rota = individual->positions[0][customer],
	     fim_rota = individual->routes_end[num_rota],
	     index = posicoes_cidades[customer];
	
	individual->capacities_free[num_rota] += load;
	individual->routes_end[num_rota]--;
	
	//Se a customer a ser removida é a última customer da route.
	if(index +1 == fim_rota)
		return;
	
	int *route = individual->routes[num_rota],
	     nova_cidade = 0;
	while(index < fim_rota -1){
		nova_cidade =  route[index +1];
		route[index] = nova_cidade;
		posicoes_cidades[nova_cidade] = index;
		index++;
	}
	return;
}



