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
    if (chemin == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    if (chdir("/Users/majda") != 0)
    {
        perror("chdir /");
        exit(EXIT_FAILURE);
    }

    getcwd(chemin, 100);
      if (chemin == NULL)
    {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        printf("\033[0;32m %s", chemin);
        input = readline("$ \033[0m");

        add_history(input);

        if (strcmp(input, "exit") == 0 || strcmp(input, "exit ") == 0)
        {
            break;
        }
        else if (strncmp(input, "cd", 2) == 0)
        {
            char *chemin_cd = input +3; //? pas besoin de le free psq il pointe vers input qui est lui meme un pointeur qui vas etre libérer a un moment
            cd_commande(chemin_cd);
            getcwd(chemin, 512);
            input = NULL;
        }
        else if (strcmp(input, "ls") == 0 || strcmp(input, "ls ") == 0)
        {
            if (fork() == 0)
            {
                execlp("ls", "ls", NULL);
                perror("execlp");
                exit(EXIT_FAILURE);
            };
            wait(NULL);
            input = NULL;
        }
        else if (strcmp(input, "pwd") == 0 || strcmp(input, "pwd ") == 0)
        {
            char *full_path = malloc(PATH_MAX);
            if (full_path == NULL)
            {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            full_path = chemin_absolu(full_path, PATH_MAX);
            fprintf(stdout, "%s\n", full_path);
            chdir(full_path);
            if (full_path == NULL)
            {
                perror("pwd");
                exit(EXIT_FAILURE);
            }
            else
            {

                printf("free de full_path\n");
                free(full_path);
            }
        }
        else
        {
            fprintf(stdout,"Commande non reconnue : %s\n", input);
            input = NULL;
        }
    }

    free(chemin);
    free(input);
    return 0;
}