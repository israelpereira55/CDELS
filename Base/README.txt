===========================================================================================

Implementação da metaheurística Evolução Diferencial aplicada à resolução 
do problema de roteamento de veículos capacitados (CVRP).

-------------------------------------------------------------------------------------------
				    DESCRIÇÃO
-------------------------------------------------------------------------------------------


A primeira linha do arquivo das demandas das cidades deve possuir quatro inteiros, 
sendo o primeiro o número de rotas, o segundo o valor ótimo da instância, o terceiro o 
número de cidades e, por fim, o quarto sendo a capacidade máxima de cada caminhão. 
A linha seguinte possuirá a primeira cidade, seguida das coordenadas x,y. O restante do 
arquivo deve ser como o arquivo original das instâncias.


Os parâmetros devem ser ajustados no arquivo EvolucaoDiferencial.h. A quantidade de 
indivíduos da geração é calculada pelo PESO * (número de cidades / número de rotas).
O parâmetro PESO pode ser modificado no arquivo EvolucaoDiferencial.h.

O NUM_MAX_GERACOES será um dos critérios de parada do programa.

CR_MAX determina o raio dos números aleatórios que coordenarão a taxa de crossover, sendo
CR a taxa de crossover na execução do programa. Os cruzamentos serão feitos se um 
número gerado aleatóriamente a partir de 0 a CR_MAX -1 for menor que o CR.

F deve ser um número real de 0-1 que determinará o número de componentes que serão 
perturbadas na mutação.

O parâmetro IMPRESSAO_ARQUIVO deve ser 0 ou 1. Ele coordenará se os dados serão mostrados 
no arquivo ou no terminal. Caso seja 0 as impressões serão feitas no terminal, senão,
o programa criará um arquivo relatorio.ed contendo o relatório das gerações e também
será criado um arquivo solucao.vrp que possuirá a melhor resposta.

===========================================================================================
