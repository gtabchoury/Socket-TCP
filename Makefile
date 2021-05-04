all: servidor cliente

servidor: servidor.c tictactoe.c tictactoe.h shared_mem.c shared_mem.h
	gcc servidor.c tictactoe.c shared_mem.c -o servidor -g -Wall

cliente: cliente.c tictactoe.c tictactoe.h shared_mem.c shared_mem.h
	gcc cliente.c tictactoe.c shared_mem.c -o cliente -g -Wall

clean:
	rm -f servidor cliente
