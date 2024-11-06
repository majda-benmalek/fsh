#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include "../../utils/cd.h"
#include "../../utils/pwd.h"

int main()
{
    char *input;
    char *chemin = malloc(100);
    getcwd(chemin, 100);
     if (chemin == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        printf("\033[0;32m %s", chemin);
        input = readline("$ \033[0m");
        
        add_history(input);

        if (strcmp(input, "exit") == 0)
        {
            break;
        }
        else if (strncmp(input, "cd", 2) == 0)
        {
            char *chemin_cd = input + 3; //? pas besoin de le free psq il pointe vers input qui est lui meme un pointeur qui vas etre libérer a un moment 
            cd_commande(chemin_cd);
            getcwd(chemin, 512);
            input = NULL;
        }
        else if (strcmp(input, "ls") == 0)
        {
            if (fork() == 0)
            {
                execlp(input, input, NULL);
                perror("execlp");
                exit(EXIT_FAILURE);
            };
            wait(NULL);
            input = NULL;
        }
        else if (strcmp(input, "pwd") == 0)
        {
            char *path = nom_du_repertoire();
            printf("%s\n", path);
            input = NULL;
            free(path);
        }
        else
        {
            printf("Commande non reconnue : %s\n", input);
            input = NULL;
        }
    }

    free(chemin);
    free(input);
    return 0;
}