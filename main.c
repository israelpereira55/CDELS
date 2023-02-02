#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include "base/local_search.h"
#include "base/dependences.h"
#include "base/arquivo.h"
#include "metaheuristic/differential_evolution.h"


/* Rodando
int main(int argc, char *argv[]) {
    F = atof(argv[1]);
    CR = atof(argv[2]);
    int semente = atoi(argv[4]);

    int customers_num = 0,
        vehicles_num = 0,
        best_solution = 0,
        capacity_max = 0,
      //  escolha = escolha_tecnica(),
        escolha = 2,
        crossover_binario = escolha % 2; //Se a escolha é impar o crossover é binario.

    if(!crossover_binario) escolha--;
    
    if(escolha == 2) {
        printf("Técnica rand/1/exp\n");
    } else  printf("Técnica divergente!\n");
    
    char nome[] = "a";

//  printf("    Escreva o nome do arquivo com os dados das customers.\n    Nome: ");
//  scanf("%s", nome);
    printf("\n");

    FILE* leitura = fopen(argv[3], "r");
    if(leitura == NULL) {
        printf("    O arquivo %s não foi encontrado no disco.\n    Abortando...\n\n", nome);
        return 0;
    }
    
    fscanf(leitura, "%d %d %d %d", &vehicles_num, &best_solution, &customers_num, &capacity_max);
    
    NP = 2 * customers_num;
    
    Cidade customers[customers_num];
    arquivo_inicializa_cidades(customers, leitura, customers_num);
    
    int** distances = matriz_custos_inicializa(customers, customers_num);

    fclose(leitura);
    
    printf("Semente: %d\n", semente); 
    srand( semente );
    
    switch(escolha) {//1 indica que o tipo de mutação é randomica (rand) e 0 utiliza o melhor individual (best).
        case 1 :
            EvolucaoDiferencial_1(distances, customers, customers_num, vehicles_num, capacity_max, best_solution, 1, crossover_binario); 
            break;
        case 3:
            EvolucaoDiferencial_1(distances, customers, customers_num, vehicles_num, capacity_max, best_solution, 0, crossover_binario);
            break;
        case 5:
            EvolucaoDiferencial_2(customers, customers_num, vehicles_num, capacity_max, best_solution, 1, crossover_binario);
            break;
        case 7:
            EvolucaoDiferencial_2(customers, customers_num, vehicles_num, capacity_max, best_solution, 0, crossover_binario);
            break;
        case 9:
            EvolucaoDiferencial_2_rand_to_best(customers, customers_num, vehicles_num, capacity_max, best_solution, crossover_binario);
            break;
    }
    
    distances = matriz_custos_libera(distances, customers_num);
    return 0;
}
*/

int main() {
    //F = 0.1;
    //CR = 0.9;
    int semente = time(NULL);
    
    printf("ED Combinatória!\n\n");

    int customers_num = 0,
        vehicles_num = 0,
        best_solution = 0,
        capacity_max = 0,
        escolha = escolha_tecnica(),
        crossover_binario = escolha % 2; //Se a escolha é impar o crossover é binario.
        
    switch(escolha){
        case 1:
            printf("Técnica rand/1/bin\n");
            break;

        case 2:
            printf("Técnica rand/1/exp\n");
            break;

        case 3:
            printf("Técnica best/1/bin\n");
            break;
        
        case 4:
            printf("Técnica best/1/exp\n");
            break;
    }

    if(!crossover_binario) escolha--;
    
    printf("F=%.2lf, CR=%.2lf, PENA=%d\n", F, CR, PENALIDADE);
    
    char nome[81];

    printf("    Escreva o nome do arquivo com os dados das customers.\n    Nome: ");
    scanf("%s", nome);

    FILE* leitura = fopen(nome, "r");
    if(leitura == NULL) {
        printf("    O arquivo %s não foi encontrado no disco.\n    Abortando...\n\n", nome);
        return 0;
    }
    
    fscanf(leitura, "%d %d %d %d", &vehicles_num, &best_solution, &customers_num, &capacity_max);
    
    //NP = 3 * customers_num;
    printf("NP=%d\n", NP);
    
    Cidade customers[customers_num];
    arquivo_inicializa_cidades(customers, leitura, customers_num);
    
    int** distances = matriz_custos_inicializa(customers, customers_num);

    fclose(leitura);
    
    printf("Semente: %d\n\n", semente); 
    srand( semente );
    
    switch(escolha) {//1 indica que o tipo de mutação é randomica (rand) e 0 utiliza o melhor individual (best).
        case 1 :
            EvolucaoDiferencial_1(distances, customers, customers_num, vehicles_num, capacity_max, best_solution, 1, crossover_binario); 
            break;
        case 3:
            EvolucaoDiferencial_1(distances, customers, customers_num, vehicles_num, capacity_max, best_solution, 0, crossover_binario);
            break;
    }
    
    distances = matriz_custos_libera(distances, customers_num);
    return 0;
}
