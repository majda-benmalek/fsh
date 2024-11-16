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

// TODO : documentation
void gestion_cmd(char *input, char **arg, char **cmd)
{
    char *espace = strchr(input, ' ');
    if (espace != NULL)
    {
        int indice_espace = espace - input;
        *arg = (input + indice_espace + 1);
    }
    if (espace != NULL && strlen(*arg) == 0)
    {
        snprintf(*cmd, strlen(input), "%s", input);
    }
    else if (espace == NULL && strlen(*arg) == 0)
    {
        sprintf(*cmd, "%s", input);
    }
    else if (strlen(*arg) >= 1)
    {
        snprintf(*cmd, (strlen(input) - strlen(*arg)), "%s", input);
    }
}

int fsh(int dernier_exit)
{
    char *input = malloc(MAX_INPUT);
    if (input == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    char *chemin = malloc(PATH_MAX);
    if (chemin == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if (getcwd(chemin, PATH_MAX) == NULL)
    {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }
    //* Ouvert fsh directement dans le répertoire personnel de l'utilisateur (à voir)
    // if (chdir(getenv("HOME"))==-1)
    // {
    //     perror("chdir");
    //     exit(EXIT_FAILURE);
    // }

    rl_outstream = stderr;
    using_history();
    read_history("history.txt");

    int ret = 0; // la valeur de retour de chaque commande (pour le prompt)

    char *arg = malloc(ARG_MAX);
    char *cmd = malloc(ARG_MAX);
    if (arg == NULL || cmd == NULL)
    {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        if (arg != NULL && cmd != NULL) // réinitialisation de arg et cmd a chaque tour de boucle
        {
            strcpy(arg, "");
            strcpy(cmd, "");
        }
        prompt(chemin, input, &ret);
        gestion_cmd(input, &arg, &cmd);
        ret = 0; // réinitialisation de ret après l'avoir utilisé dans l'affichage du prompt
        //* Commande exit
        if (strcmp(cmd, "exit") == 0)
        {
            cmd_exit(arg);
            // break;
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
        else if (strcmp(cmd, "for") == 0)
        {
            ret = boucle_for(input);
        }
        else
        {
            printf("input : %s\n", input);
            ret = cmd_extern(input);
            if (ret == 1)
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

        if (input != NULL)
    {
        free(input);
    }
    if (chemin != NULL)
    {
        free(chemin);
    }
    if (arg != NULL)
    {
        free(arg);
    }
    if (cmd != NULL)
    {
        free(cmd);
    }
    write_history("history.txt");
    return 0;
}