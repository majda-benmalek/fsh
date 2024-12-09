#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
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
#include "cmdSimple.c"
#include <linux/limits.h>
#define ARG_MAX 512

void gestion_cmd(char *input, commandeStruct *cmdstruct)
{
    if (!cmdstruct)
    {
        perror("Erreur Structure");
        return;
    }

    make_cmdSimple(input, cmdstruct);
    if (strstr(input, " | "))
    {
        make_cmdPipe(cmdstruct);
    }
}

//Devrai renvoyé une commande simple
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
int fsh(char *input, char *chemin, int *dernier_exit, commandeStruct *cmdstruct)
{
    int ret = 0;
    if (!cmdstruct || !cmdstruct->cmdSimple || !cmdstruct->cmdSimple->args[0])
    {
        perror("Structure commande");
        return -1;
    }
    // exit
    if (cmdstruct->cmdSimple->type == CMD_INTERNE)
    {
        if (strcmp(cmdstruct->cmdSimple->args[0], "exit") == 0)
        {
            *dernier_exit = commande_exit(cmdstruct->cmdSimple->args[1]);
            if (*dernier_exit == -4)
            {
                ret = -4;
                *dernier_exit = 0;
                return ret;
            }
            if (chemin)
                free(chemin);
            free(cmdstruct->cmdSimple->args);
            free(cmdstruct->cmdSimple);
            free(cmdstruct);
            exit(*dernier_exit);
        }

        // gestion de cd
        else if (strcmp(cmdstruct->cmdSimple->args[0], "cd") == 0)
        {
            ret = cd_commande(cmdstruct->cmdSimple->args[1]);
            if (getcwd(chemin, PATH_MAX) == NULL)
            {
                perror("getcwd");
                return 1;
            }
            return ret;
        }

        // gestion de pwd

        else if (strcmp(cmdstruct->cmdSimple->args[0], "pwd") == 0)
        {
            ret = pwd();
        }
        //* Redirection > et >>
        else if (strstr(input, ">>") || strstr(input, ">"))
        {
            ret = redirection(input);
            if (ret != 0)
            {
                perror("Redirection");
                return ret;
            };
        }
        else if (strcmp(cmdstruct->cmdSimple->args[0], "ftype") == 0)
        {
            ret = ftype(cmdstruct->cmdSimple->args[1]);
            if (ret > 0)
            {
                perror("ftype");
                return ret;
            }
        }
        else if (strstr(input, "for"))
        {
            ret = boucle_for(input);
            if (ret != 0)
            {
                perror("boucle_for");
                return ret;
            };
        }
        else if (cmdstruct->cmdSimple == NULL ||
                 cmdstruct->cmdSimple->args == NULL ||
                 cmdstruct->cmdSimple->args[0] == NULL ||
                 cmdstruct->cmdSimple->args[0][0] == '\0' ||
                 cmdstruct->cmdSimple->args[0][0] == '\n' ||
                 cmdstruct->cmdSimple->args[0][0] == ' ' ||
                 cmdstruct->cmdSimple->args[0][0] == '\t')
        {
            ret = *dernier_exit;
        }
    }
    else if (cmdstruct->cmdSimple->type == CMD_EXTERNE)
    {
        ret = cmd_extern(cmdstruct->cmdSimple);
        if (ret < 0)
        {
            return ret;
        }
    }

    return ret;
}

/*int fsh(char *cmd, char *arg, char *input, char *chemin, int *dernier_exit)
{
    int ret = 0;
    if (strcmp(cmd, "exit") == 0)
    {

        *dernier_exit = commande_exit(arg);
        if (*dernier_exit == -5)
        {
            ret = -5;
            *dernier_exit = 0;
            return ret;
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
        exit(*dernier_exit);
    }
    // Commande cd
    else if (strcmp(cmd, "cd") == 0)
    {
        ret = cd_commande(arg);
        if (getcwd(chemin, PATH_MAX) == NULL)
        {
            perror("getcwd");
            return 1;
        }
        return ret;
    }
    // Commande pwd
    else if (strcmp(cmd, "pwd") == 0)
    {
        ret = pwd();
    }
    // Redirection > et >>
    else if (strstr(input, ">>") || strstr(input, ">"))
    {
        ret = redirection(input);
        if (ret != 0)
        {
            perror("Redirection");
            return ret;
        };
    }
    else if (strcmp(cmd, "ftype") == 0)
    {
        ret = ftype(arg);
        if (ret > 0)
        {
            perror("ftype");
            return ret;
        }
    }
    else if (strstr(input, "for"))
    {
        ret = boucle_for(input);
        if (ret != 0)
        {
            perror("boucle_for");
            return ret;
        };
    }
    else if (cmd[0] == '\0' || cmd[0] == '\n' || cmd[0] == ' ' || cmd[0] == '\t')
    {
        ret = *dernier_exit;
    }
    else
    {
        ret = cmd_extern(input);
        if (ret < 0)
        {
            return ret;
        }
    }
    return ret;
}*/