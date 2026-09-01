CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

all: orquestrador relay

orquestrador: orquestrador.c
	$(CC) $(CFLAGS) -o $@ orquestrador.c

relay: relay.c
	$(CC) $(CFLAGS) -o $@ relay.c

solucao: orquestrador-solucao relay-solucao

orquestrador-solucao: orquestrador-solution.c
	$(CC) $(CFLAGS) -o $@ orquestrador-solution.c

relay-solucao: relay-solution.c
	$(CC) $(CFLAGS) -o $@ relay-solution.c

# 'make entregavel' empacota os fontes num diretorio 'entregavel/'
# dentro de entregavel.tgz, pronto para extrair e compilar em outra
# maquina (tar xzf entregavel.tgz && cd entregavel && make).
entregavel: entregavel.tgz

entregavel.tgz: Makefile orquestrador.c relay.c
	rm -rf entregavel
	mkdir entregavel
	cp Makefile orquestrador.c relay.c entregavel/
	tar czf $@ entregavel
	rm -rf entregavel

clean:
	rm -f orquestrador orquestrador-solucao relay relay-solucao entregavel.tgz
	rm -rf entregavel

.PHONY: all solucao entregavel clean
