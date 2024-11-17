#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include "../../utils/cd.h"
#include "../../utils/pwd.h"
#include "../../utils/exit.h"
#include "../../utils/redirection.h"
#include "../../utils/prompt.h"
#include <stdbool.h>
#include "../../utils/gestion.h"
#include "../../utils/extern.h"
#include "../../utils/for.h"

#define ARG_MAX 512

int dernier_exit = 0; // pour initialiser la derniére valeur de retour

int main(void)
{
    char *input = malloc(MAX_INPUT);
    if (input == NULL)
    {
        perror("malloc");
        return 1;
    }

    char *chemin = malloc(PATH_MAX);
    if (chemin == NULL)
    {
        perror("malloc");
        return 1;
    }

    if (getcwd(chemin, PATH_MAX) == NULL)
    {
        perror("getcwd");
        return 1;
    }
    rl_outstream = stderr;
    using_history();
    read_history("history.txt");

    int ret = 0;

    char *arg = malloc(ARG_MAX);
    char *cmd = malloc(ARG_MAX);
    if (arg == NULL || cmd == NULL)
    {
        write(2, "Erreur d'allocation de mémoire\n",strlen("Erreur d'allocation de mémoire\n"));
        return 1;
    }

    while (1)
    {
        prompt(chemin, input, &ret);
        gestion_cmd(input, &arg, &cmd);
        dernier_exit = ret;

        //* Commande exit
        if (strcmp(cmd, "exit") == 0)
        {
            ret = commande_exit(arg);
            if (input != NULL)
            {
                free(input);
            }
            if (chemin != NULL)
            {
                free(chemin);
            }
            return ret;
        } // pour le exit si on met une autre valeur que le 0 le makefile affiche une erreur ce qui esr normal mais y'a possiblité de changer ca (demander si c'est nécessaire)
        //* Commande cd
        else if (strcmp(cmd, "cd") == 0)
        {
            ret = cd_commande(arg);
            getcwd(chemin, 512); // met le nouveau chemin dans la variable chemin
        }
        //* Commande pwd
        else if (strcmp(cmd, "pwd") == 0)
        {
            ret = pwd();
        }
        //* Redirection > et >>
        else if (strstr(input, ">>") || strstr(input, ">"))
        {
            // printf("detection de > >> \n");
            ret = redirection(input);

            if (ret != 0)
            {
                write(2, "Redirection échouée\n", strlen("Redirection échouée\n"));
            }
        }
        //* Commande externe ls sans argument
        else if (strcmp(cmd, "ls") == 0)
        {
            if (fork() == 0)
            {
                ret = execlp("ls", "ls", NULL);
                if (ret == -1)
                {
                    perror("execlp");
                    return 1;
                }
            }
            wait(NULL);
        }
        else if (strstr(input, ">>") || strstr(input, ">"))
        {
            // printf("detection de > >> \n");
            int result = redirection(input);

            if (result != 0)
            {
                printf("Redirection échouée\n");
                input = NULL;
            }

            input = NULL;
        }
        else
        {
            char *msg = malloc(MAX_INPUT);
            sprintf(msg, "Commande non reconnue : %s\n", input);
            write(2, msg, strlen(msg));
            ret = 1;
            if (msg != NULL)
            {
                free(msg);
            }
        }
    }
}
