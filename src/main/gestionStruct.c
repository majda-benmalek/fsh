#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "../../utils/commande.h"
#define ARG_MAX 512

commandeStruct *remplissage_cmdStruct(Type type, cmd_simple *cmdSimple, cmd_pipe *pipestruct, cmdIf *cmdIfStruct, cmdFor *cmdForStruct, int nbcommandes)
{

    commandeStruct *cmd = malloc(sizeof(commandeStruct));

    cmd->type = type;
    cmd->cmdSimple = cmdSimple;
    cmd->pipe = pipestruct;
    cmd->cmdIf = cmdIfStruct;
    cmd->cmdFor = cmdForStruct;
    cmd->nbCommandes = nbcommandes;

    return cmd;
}
void freeCmdSimple(cmd_simple *cmd)
{
    if (!cmd)
        return;

    if (cmd->args)
    {
        for (char **arg = cmd->args; *arg; ++arg)
        {
            free(*arg);
        }
        free(cmd->args);
    }
    free(cmd);
}

void freePipe(cmd_pipe *pipeStruct)
{
    if (!pipeStruct)
        return;
    for (int i = 0; i < pipeStruct->nbCommandes; i++)
    {
        free(pipeStruct->commandes[i]);
    }
    free(pipeStruct->commandes);
    free(pipeStruct);
}

void freeCmdStruct(commandeStruct *cmd)
{
    if (!cmd)
        return;
    if (cmd->cmdSimple)
    {
        freeCmdSimple(cmd->cmdSimple);
    }

    if (cmd->pipe)
    {
        freePipe(cmd->pipe);
    }
    if (cmd->cmdIf)
    {
        if (cmd->cmdIf->commandeIf)
        {
            for (int i = 0; cmd->cmdIf->commandeIf[i] != NULL; i++)
            {
                freeCmdStruct(cmd->cmdIf->commandeIf[i]);
            }
            free(cmd->cmdIf->commandeIf);
        }
        if (cmd->cmdIf->commandeElse)
        {
            for (int i = 0; cmd->cmdIf->commandeElse[i] != NULL; i++)
            {
                freeCmdStruct(cmd->cmdIf->commandeElse[i]);
            }
            free(cmd->cmdIf->commandeElse);
        }
        if (cmd->cmdIf->test)
        {
            freePipe(cmd->cmdIf->test);
        }

        free(cmd->cmdIf);
    }

    if (cmd->cmdFor)
    {
        if (cmd->cmdFor->op)
        {
            for (char **op = cmd->cmdFor->op; *op; ++op)
            {
                free(*op);
            }
            free(cmd->cmdFor->op);
        }
        if (cmd->cmdFor->cmd)
        {
            for (int i = 0; i < cmd->cmdFor->nbCommandes; i++)
            {
                freeCmdStruct(cmd->cmdFor->cmd[i]);
            }
            free(cmd->cmdFor->cmd);
        }
        free(cmd->cmdFor->rep);
        free(cmd->cmdFor);
    }

    free(cmd);
}

cmd_simple *remplissage_cmdSimple(char **args)
{
    cmd_simple *cmd = malloc(sizeof(cmd_simple));

    if (!cmd)
    {
        perror("malloc CommandSimple");
        return NULL;
    }

    int nbargs = 0;
    while (args[nbargs])
    {
        nbargs++;
    }
    cmd->args = malloc((nbargs + 1) * sizeof(char *));
    if (!cmd->args)
    {
        perror("malloc args");
        free(cmd);
        return NULL;
    }

    for (int i = 0; i < nbargs; i++)
    {
        cmd->args[i] = strdup(args[i]);
        if (!cmd->args[i])
        {
            perror("strdup");
            for (int j = 0; j < i; j++)
            {
                free(cmd->args[j]);
            }
            free(cmd->args);
            free(cmd);
            return NULL;
        }
    }

    cmd->args[nbargs] = NULL;

    if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "pwd") == 0 || strcmp(args[0], "ftype") == 0 || strcmp(args[0], "exit") == 0)
    {
        cmd->type = CMD_INTERNE;
    }
    else
    {
        cmd->type = CMD_EXTERNE;
    }
    return cmd;
}

size_t tailleArgs(char **args)
{
    size_t taille = 0;
    while (args[taille] != NULL)
    {
        taille++;
    }
    return taille;
}

char **arg_cmdsimple_redirection(char **args)
{
    char *commande[8];
    memset(commande, 0, sizeof(commande));
    size_t size = tailleArgs(args) - 2;
    for (int i = 0; i < size; i++)
    {
        commande[i] = args[i];
    }
    return commande;
}

cmd_redirection *remplissageCmdRedirection(char **args)
{
    size_t taille = tailleArgs(args);
    cmd_redirection *cmd = malloc(sizeof(cmd_redirection));
    if (cmd == NULL)
    {
        perror("malloc");
        return NULL;
    }

    cmd->type = REDIRECTION;

    if (strstr(args[1], "<") != NULL)
    {
        cmd->fichier = args[0];
        cmd->separateur = "<";
        char *commande[8];
        size_t size = taille - 2;
        for (int i = 0; i < size; i++)
        {
            commande[i] = args[taille - size + i];
        }
        cmd->cmd = remplissage_cmdSimple(&commande);
    }
    else
    {
        char *commande = arg_cmdsimple_redirection(args);
        size_t pos_sep = tailleArgs(commande);
        cmd->cmd = remplissage_cmdSimple(&commande);
        if (strstr(args[pos_sep], ">") != NULL)
        {
            cmd->fichier = args[taille - 1]; // l'avant dernier élement vu que le dernier est NULL
            cmd->separateur = ">";
        }
        else if (strstr(args[pos_sep], " >> ") != NULL)
        {
            cmd->fichier = args[taille - 1];
            cmd->separateur = ">>";
        }
        else if (strstr(args[pos_sep], " 2> ") != NULL)
        {
            cmd->fichier = args[taille - 1];
            cmd->separateur = "2>";
        }
        else if (strstr(args[pos_sep], " >| ") != NULL)
        {
            cmd->fichier = args[taille - 1];
            cmd->separateur = " >| ";
        }
        else if (strstr(args[pos_sep], "2>|") != NULL)
        {
            cmd->fichier = args[taille - 1];
            cmd->separateur = "2>|";
        }
        else if (strstr(args[pos_sep], " 2>> ") != NULL)
        {
            cmd->fichier = args[taille - 1];
            cmd->separateur = "2>>";
        }
    }
    return cmd;
}

void free_redirection(cmd_redirection *cmd)
{
    if (!cmd)
    {
        return;
    }
    else
    {
        // free(cmd->fichier); pas de malloc donc normalement pas de free
        // free(cmd->separateur); pareil a voir
        freeCmdSimple(cmd->cmd);
        free(cmd);
    }
}
