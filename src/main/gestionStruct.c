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
size_t tailleArgs(char **args);

commandeStruct *remplissage_cmdStruct(Type type, cmd_simple *cmdSimple, cmd_pipe *pipestruct, cmdIf *cmdIfStruct, cmdFor *cmdForStruct , cmd_redirection* cmdredirection, int nbcommandes, commandeStruct *cmd)
{

    if (cmd == NULL)
    {
        cmd = malloc(sizeof(commandeStruct));
    }
    cmd->type = type;
    cmd->cmdSimple = cmdSimple;
    cmd->pipe = pipestruct;
    cmd->cmdIf = cmdIfStruct;
    cmd->cmdFor = cmdForStruct;
    cmd->cmdRed = cmdredirection;
    cmd->nbCommandes = nbcommandes;

    return cmd;
}
void freeCmdSimple(cmd_simple *cmd)
{
    if (cmd == NULL)
        return;

    if (cmd->args != NULL)
    {
        for (char **arg = cmd->args; *arg; ++arg)
        {
            if (*arg != NULL)
            {
                free(*arg);
            }
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
    if (cmd != NULL)
    {
        if (cmd->cmdSimple != NULL)
        {
            freeCmdSimple(cmd->cmdSimple);
        }

        if (cmd->pipe != NULL)
        {
            freePipe(cmd->pipe);
        }
        free(cmd);
    }
}

cmd_simple *remplissage_cmdSimple(char **args)
{
    cmd_simple *cmd = malloc(sizeof(cmd_simple));

    if (cmd == NULL)
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
    if (cmd->args == NULL)
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

int arg_cmdsimple_redirection(char **args, char **commande)
{
    size_t size = tailleArgs(args) - 2;
    for (int i = 0; i < size; i++)
    {
        commande[i] = strdup(args[i]);
        if (commande[i] == NULL)
        {
            perror("strdup arg_cmdsimple_redirection ");
            return 1;
        }
    }
    return 0;
}

void free_redirection(cmd_redirection *cmd)
{
    if (!cmd)
    {
        return;
    }
    else
    {
        if (cmd->cmd != NULL)
        {
            freeCmdSimple(cmd->cmd);
        }
        free(cmd);
    }
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
    // TODO tableau dynmaique
    char *commande[10];
    memset(commande, 0, sizeof(commande));
    if (strstr(args[1], "<") != NULL)
    {
        cmd->fichier = args[0];
        cmd->separateur = "<";
        size_t size = taille - 2;
        for (int i = 0; i < size; i++)
        {
            commande[i] = strdup(args[taille - size + i]);
            if (commande[i] == NULL)
            {
                free_redirection(cmd);
                return NULL;
            }
        }
        cmd->cmd = remplissage_cmdSimple(commande);
    }
    else
    {
        if (arg_cmdsimple_redirection(args, commande) == 1)
        {
            perror("strdup cmd_redirection");
            free_redirection(cmd);
            return NULL;
        }
        size_t pos_sep = tailleArgs(commande);
        cmd->cmd = remplissage_cmdSimple(commande);
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

int arg_cmdsimple_pipe(char **args, char **commande, int i, int j)
{
    for (int h = 0; h < (i - j); h++)
    {
        commande[h] = strdup(args[j + h]);
        if (commande[h] == NULL)
        {
            return 1;
        }
    }
    commande[i-j] = NULL; // pour le dernier élementp
    return 0;
}

void free_pipe(cmd_pipe *cmd)
{
    for (int i = 0; i < cmd->nbCommandes; i++)
    {
        if (cmd->commandes[i] != NULL)
        {
            freeCmdSimple(cmd->commandes[i]);
        }
    }
    free(cmd);
}

cmd_pipe *remplissageCmdPipe(char **args)
{
    cmd_pipe *cmd = malloc(sizeof(cmd_pipe));
    cmd->commandes = malloc(40 * sizeof(cmd_simple));
    int nb = 0;
    int j = 0;

    // TODO tableau dynamique
    char *commande[10];
    memset(commande, 0, sizeof(commande));
    for (size_t i = 0; i <= tailleArgs(args); i++)
    {
        memset(commande, 0, sizeof(commande));
        if (args[i] == NULL)
        {
            if (arg_cmdsimple_pipe(args, commande, i, j) == 1)
            {
                free_pipe(cmd);
                return NULL;
            }
            cmd->commandes[nb] = remplissage_cmdSimple(commande);
            if (cmd->commandes[nb] == NULL)
            {
                perror("remplissage cmd simple dans remplissage pipe");
                free_pipe(cmd);
                return NULL;
            }
            nb += 1;
            j = i + 1;
        }
        else if (strcmp(args[i], "|") == 0)
        {
            if (arg_cmdsimple_pipe(args, commande, i, j) == 1)
            {
                free_pipe(cmd);
                return NULL;
            }
            cmd->commandes[nb] = remplissage_cmdSimple(commande);
            if (cmd->commandes[nb] == NULL)
            {
                perror("remplissage cmd simple dans remplissage pipe");
                free_pipe(cmd);
                return NULL;
            }
            nb += 1;
            j = i + 1;
        }
    }
    cmd->type = PIPE;
    cmd->nbCommandes = nb;
    cmd->commandes = (cmd_simple **)realloc(cmd->commandes, cmd->nbCommandes * sizeof(cmd_simple *));
    return cmd;
}
// si vous voulez teste les pipes 
// cat fichier.txt | sort | head -n 5 | ftype fichier.txt
//  cat fichier.txt | sort | head -n 5