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
#include "../../utils/gestion.h"
#include "../../utils/for.h"
#define ARG_MAX 512

void freeCmdStruct(commandeStruct *cmd);

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

void free_pipe(cmd_pipe *cmd)
{
    if (cmd != NULL)
    {
        if (cmd->commandes != NULL)
        {
            for (int i = 0; cmd->commandes[i] != NULL; i++)
            {
                if (cmd->commandes[i] != NULL)
                {
                    freeCmdSimple(cmd->commandes[i]);
                }
            }
        }
        free(cmd->commandes);
        free(cmd);
    }
}

void free_for(cmdFor *cmdFor)
{
    if (cmdFor != NULL)
    {
        if (cmdFor->rep != NULL)
        {
            free(cmdFor->rep);
        }
        if (cmdFor->variable != NULL)
        {
            free(cmdFor->variable);
        }
        if (cmdFor->op != NULL)
        {
            int i = 0;
            while (cmdFor->op[i] != NULL)
            {
                free(cmdFor->op[i]);
                i++;
            }
            free(cmdFor->op);
        }
        if (cmdFor->cmd != NULL)
        {
            freeCmdStruct(cmdFor->cmd);
        }
        free(cmdFor);
    }
}

void free_if(cmdIf *cmd)
{
    if (cmd != NULL)
    {
        if (cmd->test != NULL)
        {
            freeCmdStruct(cmd->test);
        }
        if (cmd->commandeIf != NULL)
        {
            freeCmdStruct(cmd->commandeIf);
        }
        if (cmd->commandeElse != NULL)
        {
            freeCmdStruct(cmd->commandeElse);
        }
        free(cmd);
    }
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
            free_pipe(cmd->pipe);
        }

        if (cmd->cmdFor != NULL)
        {
            free_for(cmd->cmdFor);
        }
        if (cmd->cmdRed != NULL)
        {
            free_redirection(cmd->cmdRed);
        }
        if (cmd->cmdsStruc != NULL)
        {
            for (int i = 0; cmd->cmdsStruc[i] != NULL; i++)
            {
                freeCmdStruct(cmd->cmdsStruc[i]);
            }
            free(cmd->cmdsStruc);
        }
        if (cmd->cmdIf != NULL)
        {
            free_if(cmd->cmdIf);
        }
        free(cmd);
    }
}