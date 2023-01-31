#ifndef DEPENDENCIES_H_
#define DEPENDENCIES_H_

typedef struct customer Customer;

struct customer {
	int id, // Customer number
	    demand; 
	double x,y; // Customer 2D position
};


typedef struct individual Individual;

struct individual {
	int **routes,
	    **positions, // Dado um indice i no vetor posicao, o conteúdo é a posicao da customer i no vetor percurso. Ou seja, ele indica a posicao das customers no vetor percurso.
	     *fim_rotas,
	     *free_loads;
	int cost,
	    feasible,
	    cloned;
};


Customer* customer_create(int id, double x, double y);

Customer* customer_free(Customer* customer);

int** distances_matrix_init(Customer* customers, int num_customers);

int** distances_matrix_free(int** distances, int num_customers);

//int calculate_distance(Customer* c1, Customer* c2);


Individual* individual_init(int num_customers, int num_vehicles);

Individual* individual_generate_top_to_down(int** distances, Customer* customers, int num_customers, int max_capacity, int num_vehicles);

Individual* individual_gererate_down_to_top(int** distances, Customer* customers, int num_customers, int max_capacity, int num_vehicles);

void individual_swap_customers(Individual* individual, int customer1, int load1, int customer2, int load2);

void individual_reinsert_customer_in_route(Individual* individual, int customer, int new_index);

void individual_insert_customer(Individual* individual, int customer, int load, int nova_posicao, int new_route);


void individual_remove_customer(Individual* individual, int customer, int load);

void individual_update_attributes(Individual* individual, int max_capacity, int num_vehicles, int** distances, Customer* customers);

Individual* individual_clone(Individual* individual, int num_customers, int num_vehicles);

Individual* individual_free(Individual* individual, int num_vehicles);


#endif /*DEPENDENCIES_H_*/
