#ifndef DEPENDENCES_H
#define DEPENDENCES_H

typedef struct customer Customer;

struct customer {
	int	id,		/* Customer number */
		demand; /* Customer demand */
	double x,y; /* Customer 2D position */
};


typedef struct individual Individual;

struct individual {
	int **routes,		/* Matrix of vector, each vector represents a route */
	    **positions,	/* 2D vector. Each index i presents the position of the customer i in routes. First vector presents the route number and second presents the route index */
	     *routes_end,	/* Number of customers each route contains */
	     *capacities_free;	/* Available load/demand each route/vehicle has */
	int cost,			/* Individual cost/objective function */
	    feasible,		/* A flag to represent if the individual is feasible */ //TODO: make enum?
	    cloned;			/* A flag to represent if the individual is cloned */ //TODO: define cloned better; SOFT CLONE AND HARD CLONE
};


Customer* customer_init(int id, double x, double y);

Customer* customer_free(Customer* customer);

int** distances_matrix_init(Customer* customers, int customers_num);

int** distances_matrix_free(int** distances, int customers_num);

//int calculate_distance(Customer* c1, Customer* c2);


Individual* individual_init(int customers_num, int vehicles_num);

Individual* individual_free(Individual* individuo, int vehicles_num);

Individual* individual_generate_top_to_down(int** distances, Customer* customers, int customers_num, int capacity_max, int vehicles_num);

Individual* individual_generate_down_to_top(int** distances, Customer* customers, int customers_num, int capacity_max, int vehicles_num);

void individual_swap_customers(Individual* individual, int customer1, int load1, int customer2, int load2);

void individual_reinsert_customer_in_route(Individual* individual, int customer, int new_idx);

void individual_insert_customer(Individual* individual, int customer, int load, int new_idx, int new_route);

void individual_remove_customer(Individual* individual, int customer, int load);

void individual_reevaluate(Individual* individual, int capacity_max, int vehicles_num, int** distances, Customer* customers);

Individual* individual_make_hard_clone(Individual* individual, int customers_num, int vehicles_num);


#endif /* DEPENDENCES_H */
