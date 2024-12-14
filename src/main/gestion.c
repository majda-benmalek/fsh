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
#include "../../utils/gestionStruct.h"
#include "../../utils/redirection.h"
#include "../../utils/pipe.h"
#define ARG_MAX 512

// static int redirection_en_cours = 0; // Indicateur pour la redirection en cours

int rechercheDansArgs(char *tofind, char **args)
{
    for (int i = 0; i < tailleArgs(args) - 1; i++)
    {
        if (strcmp(args[i], tofind) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void gestion_cmd(char **args, commandeStruct *cmdstruct)
{
    if (!cmdstruct)
    {
        perror("Erreur Structure");
        return;
    }
    if (args[0] == NULL)
    {
        return;
    }
    else if (rechercheDansArgs(">", args) || rechercheDansArgs(">>", args) || rechercheDansArgs("<", args) || rechercheDansArgs(">|", args) || rechercheDansArgs("2>", args) || rechercheDansArgs("2>>", args) || rechercheDansArgs("2>|", args))
    {
        cmdstruct->cmdRed = remplissageCmdRedirection(args);
        cmdstruct->type = REDIRECTION;
        if (cmdstruct->cmdRed == NULL)
        {
            perror("Erreur remplissage redirection");
        }
    }
    else if (rechercheDansArgs("|", args))
    {
        cmdstruct->pipe = remplissageCmdPipe(args);
        cmdstruct->type = PIPE;
        if (cmdstruct->pipe == NULL)
        {
            perror("erreur remplissage pipe");
        }
    }
    else
    {
        cmdstruct->cmdSimple = remplissage_cmdSimple(args);
        if (cmdstruct->cmdSimple->type)
        {
            cmdstruct->type = CMD_INTERNE;
        }
        else
        {
            cmdstruct->type = CMD_EXTERNE;
        }
        if (!cmdstruct->cmdSimple)
        {
            perror("Erreur cmdSimple");
        }
    }
}

int exec_redirection(cmd_redirection *cmd)
{
    if (cmd == NULL)
    {
        perror("Structure redirection vide dans exec ");
        return -1;
    }
    return redirection(cmd);
}

int exec_pipe(cmd_pipe *cmd)
{
    return cmdpipe(cmd);
}

int fsh(char *chemin, int *dernier_exit, commandeStruct *cmdstruct)
{
    int ret = *dernier_exit;

    /*gestion de la commande Simple pour l'instant cad CMD_INTERNE && CMD_EXTERNE*/
    if (cmdstruct == NULL)
    {
        perror("Structure commande");
        return -1;
    }
    // exit
    // TODO testé direct si cmdstruct->type = CMD_INTERNE sinon problème psq si cmd==NULL erreur
    if (cmdstruct->type == CMD_INTERNE)
    {
        char *cmd = cmdstruct->cmdSimple->args[0];
        char *arg = cmdstruct->cmdSimple->args[1];
        if (strcmp(cmd, "exit") == 0)
        {
            if (cmdstruct->cmdSimple->args[2] != NULL)
            {
                write(2, "exit: too many arguments\n", strlen("exit: too many arguments\n"));
                ret = 1;
                return ret;
            }
            *dernier_exit = commande_exit(arg);
            if (chemin)
                free(chemin);
            if (cmdstruct)
                freeCmdStruct(cmdstruct);
            exit(*dernier_exit);
        }

        // gestion de cd
        else if (strcmp(cmd, "cd") == 0)
        {
            if (cmdstruct->cmdSimple->args[2] != NULL)
            {
                write(2, "cd: too many arguments\n", strlen("cd: too many arguments\n"));
                ret = 1;
                return ret;
            }
            ret = cd_commande(arg);
            if (getcwd(chemin, PATH_MAX) == NULL)
            {
                perror("getcwd");
                ret = 1;
                return ret;
            }
            return ret;
        }
        // gestion de pwd
        else if (strcmp(cmd, "pwd") == 0)
        {
            if (cmdstruct->cmdSimple->args[1] != NULL)
            {
                write(2, "pwd: too many arguments\n", strlen("pwd: too many arguments\n"));
                ret = 1;
                ret = 1;
                return ret;
            }
            ret = cd_commande(arg);
            if (getcwd(chemin, PATH_MAX) == NULL)
            {
                perror("getcwd");
                ret = 1;
                return ret;
            }
            return ret;
        }
        // gestion de pwd
        else if (strcmp(cmd, "pwd") == 0)
        {
            if (cmdstruct->cmdSimple->args[1] != NULL)
            {
                write(2, "pwd: too many arguments\n", strlen("pwd: too many arguments\n"));
                ret = 1;
                return ret;
            }
            ret = pwd();
        }
        else if (strcmp(cmd, "ftype") == 0)
        {
            if (cmdstruct->cmdSimple->args[2] != NULL)
            {
                write(2, "ftype: too many arguments\n", strlen("ftype: too many arguments\n"));
                ret = 1;
                return ret;
            }
            ret = ftype(arg);
            if (ret > 0)
            {
                perror("ftype");
                return ret;
            }
        }
    }
    else if (cmdstruct->type == REDIRECTION)
    {
        if (cmdstruct->cmdRed == NULL)
        {
            perror("Structure redirection vide dans fsh ");
            return -1;
        }
        ret = exec_redirection(cmdstruct->cmdRed);
    }
    else if (cmdstruct->type == PIPE)
    {
        ret = exec_pipe(cmdstruct->pipe);
    }
    else if (cmdstruct->type == CMD_EXTERNE)
    {
        ret = cmd_extern(cmdstruct->cmdSimple);
        if (ret < 0)
        {
            return ret;
        }
    }
    return ret;
}