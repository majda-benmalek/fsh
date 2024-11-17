SRC = src/main/fsh.c src/commandes_internes/cd.c  src/commandes_internes/pwd.c src/commandes_internes/exit.c src/commandes_internes/redirection.c src/commandes_externes/for.c src/main/prompt.c src/main/gestion.c src/commandes_externes/extern.c
EXEC = fsh

all: $(EXEC)

$(EXEC): $(SRC)
	gcc -Wall -o $(EXEC) $(SRC) -lreadline

clean:
	rm -f $(EXEC)

run: 
	exec/./main

.PHONY: all clean 
