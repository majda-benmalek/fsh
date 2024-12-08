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

#define ARG_MAX 512

void gestion_cmd(char *input, char *arg, char *cmd)
{
    char *espace = strchr(input, ' ');
    int indice_espace = espace - input;
    arg[0] = '\0';
    cmd[0] = '\0';
    if (input[0] == '\0')
    {
        sprintf(cmd, "%c", '\0');
    }
    if (espace != NULL && input[indice_espace + 1] != '\0') // ya un espace et un argument
    {
        snprintf(arg, strlen(input) - indice_espace, "%s", input + indice_espace + 1);
    }
    if (espace != NULL && strlen(arg) == 0) // ya un espace mais pas d'argument
    {
        snprintf(cmd, strlen(input), "%s", input); // un espace et sans argument
    }
    if (espace == NULL)
    {
        snprintf(cmd, strlen(input) + 2, "%s", input); //pas d'espace et pas d'argument
    }
    if (strlen(arg) >= 1)
    {
        snprintf(cmd, (strlen(input) - strlen(arg)), "%s", input); //pas d'espace apres l'argument
    }
}

int fsh(char *cmd, char *arg, char *input, char *chemin, int *dernier_exit)
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
    //* Commande cd
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
    //* Commande pwd
    else if (strcmp(cmd, "pwd") == 0)
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
}
