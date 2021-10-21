# Makefile: Implementação da metaheurística Evolução Diferencial para resolução do problema de roteamento.
# Aluno: Israel Pereira de Souza.
# Professora: Maria Cristina Rangel.
# UFES 2016


# Variaveis

nome_programa = ED
diretorioED = Combinatoria
main = EvolucaoDiferencial

dependencia1 = dependencias
dependencia2 = arquivo
dependencia3 = BuscaLocal

#Compilação

all: build

build:
	@gcc -c Base/*.c -O3 -Wall
	@gcc -c $(diretorioED)/$(main).c
	@gcc -o $(nome_programa) *.o -lm
	@rm *.o
	@echo "Compilado!"
run:
	clear
	./ED

clean:
	rm $(nome_programa)
	
debug:
	clear
	gcc -c  $(main).c $(dependencia1).c $(dependencia2).c $(dependencia3).c -Og -Wall -g
	gcc -o $(nome_programa) $(main).o  $(dependencia1).o $(dependencia2).o $(dependencia3).o -lm
	rm *.o
	valgrind ./ED
