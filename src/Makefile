CC = gcc
FLAGS = -g -Wall -Werror
OBJ = handle_client.o zroxy.o

zroxy: $(OBJ)
	$(CC) -o $@ $^ $(FLAGS)

clean:
	rm -rf *.o