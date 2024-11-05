SRC = src/main/main.c src/commandes_internes/cd.c  src/commandes_internes/pwd.c
EXEC = exec/main

all: $(EXEC)

$(EXEC): $(SRC)
	gcc -Wall -o $(EXEC) $(SRC) -lreadline

clean:
	rm -f $(EXEC)

run: 
	exec/./main

cd : 
	gcc -Wall -o exec/cd src/commandes_internes/cd.c

run_cd : 
	exec/./cd
.PHONY: all clean