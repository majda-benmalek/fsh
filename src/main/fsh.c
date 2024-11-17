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

    int ret;

    char *arg = malloc(ARG_MAX);
    char *cmd = malloc(ARG_MAX);
    if (arg == NULL || cmd == NULL)
    {
        write(2, "Erreur d'allocation de mémoire\n", strlen("Erreur d'allocation de mémoire\n"));
        return 1;
    }

    while (1)
    {
        prompt(chemin, input, &ret);
        if (strlen(input) == 0)
        {
            continue;
        }
        gestion_cmd(input, &arg, &cmd);
        // dernier_exit = ret;
        // ret = 0;
        //* Commande exit
        if (strcmp(cmd, "exit") == 0)
        {
            dernier_exit = commande_exit(arg);

            write_history("history.txt");
            if (input != NULL)
            {
                free(input);
            }
            if (chemin != NULL)
            {
                free(chemin);
            }
            exit(dernier_exit);
        }
        //* Commande cd
        if (strcmp(cmd, "cd") == 0)
        {
            ret = cd_commande(arg);
            dernier_exit = ret;
            if (getcwd(chemin, PATH_MAX) == NULL)
            {
                perror("getcwd");
                return 1;
            }
            continue;
        }
        //* Commande pwd
        if (strcmp(cmd, "pwd") == 0)
        {
            ret = pwd();
            dernier_exit = ret;
            continue;
        }
        //* Redirection > et >>
        if (strstr(input, ">>") || strstr(input, ">"))
        {
            ret = redirection(input);
            dernier_exit = ret;
            if (ret != 0)
            {
                write(2, "Redirection échouée\n", strlen("Redirection échouée\n"));
            }
            continue;
        }
        if (strchr(cmd, '\n') == 0)
        {
            continue;
        }

        // fprintf(stderr, "input avant cmd_extern : %s\n", input);
        // ret = cmd_extern(input);
        // if (ret >= 1)
        // {
        //     char *msg = malloc(MAX_INPUT);
        //     sprintf(msg, "Commande non reconnue : %s\n", cmd);
        //     write(2, msg, strlen(msg));
        //     ret = 1;
        //     if (msg != NULL)
        //     {
        //         free(msg);
        //     }
        // }
        // continue;
    }
}
