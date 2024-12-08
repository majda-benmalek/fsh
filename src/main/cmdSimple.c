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
#include "../../utils/ftype.h"
#include "../../utils/commande.h"
#define ARG_MAX 512

void make_cmdSimple(char *input, commandeStruct *cmdstruct)
{
    cmdstruct->cmdSimple = malloc(sizeof(cmd_simple));
    if (!cmdstruct->cmdSimple)
    {
        perror("Erreur allocation mémoire de la commande simple");
        return;
    }

    char *token = strtok(input, " \t"); // pour gerer le cas ou l'utilisateur separe les arguments avec tab
    int nb_args = 0;
    while (token != NULL && nb_args < ARG_MAX - 1)
    {
        cmdstruct->cmdSimple->args = realloc(cmdstruct->cmdSimple->args, sizeof(char *) * (nb_args + 1)); // le tab args va etre aggrandit dynamiquement selon le nbargs
        if (!cmdstruct->cmdSimple->args)
        {
            perror("Erreur allocation mémoire de la commande simple");
            for (int i = 0; i < nb_args; i++)
            {
                free(cmdstruct->cmdSimple->args[i]);
            }
            free(cmdstruct->cmdSimple);
            return;
        }
        cmdstruct->cmdSimple->args[nb_args] = strdup(token);

        if (!cmdstruct->cmdSimple->args[nb_args])
        {
            perror("Erreur lors de laDuplication de la commande");
            for (int i = 0; i < nb_args; i++)
            {
                free(cmdstruct->cmdSimple->args[i]);
            }
            free(cmdstruct->cmdSimple->args);
            free(cmdstruct->cmdSimple);
            return;
        }
        token = strtok(NULL, " \t");
        nb_args++;
    }

    cmdstruct->cmdSimple->args = realloc(cmdstruct->cmdSimple->args, sizeof(char *) * (nb_args + 1));
    cmdstruct->cmdSimple->args[nb_args] = NULL;
    if (cmdstruct->cmdSimple->args[0])
    {
        if (strcmp(cmdstruct->cmdSimple->args[0], "exit") == 0 ||
            strcmp(cmdstruct->cmdSimple->args[0], "cd") == 0 ||
            strcmp(cmdstruct->cmdSimple->args[0], "pwd") == 0 ||
            strcmp(cmdstruct->cmdSimple->args[0], "ftype") == 0)
        {
            cmdstruct->cmdSimple->type = CMD_INTERNE;
        }
        else
        {
            cmdstruct->cmdSimple->type = CMD_EXTERNE;
        }
    }
}