SRC = src/main/main.c src/commandes_internes/cd.c  src/commandes_internes/pwd.c src/commandes_internes/exit.c src/commandes_internes/redirection.c  src/main/prompt.c
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

exit :
	gcc -Wall -o exec/exit src/commandes_internes/exit.c

run_exit :
	exec/./exit

prompt :
	gcc -Wall -o exec/prompt src/main/prompt.c -lreadline

run_prompt :
	exec/./prompt


.PHONY: all clean