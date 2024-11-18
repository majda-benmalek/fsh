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

// TODO : documentation
void gestion_cmd(char *input, char **arg, char **cmd)
{
    char *espace = strchr(input, ' ');
    if (input[0] == '\0')
    {
        sprintf(*cmd, "%c", '\0');
    }
    if (espace != NULL)
    {
        int indice_espace = espace - input;
        *arg = (input + indice_espace + 1);
    }
    if (espace != NULL && strlen(*arg) == 0)
    {
        snprintf(*cmd, strlen(input), "%s", input);
    }
    else if (espace == NULL)
    {
        snprintf(*cmd, strlen(input) + 1, "%s", input);
    }
    else if (strlen(*arg) >= 1)
    {
        snprintf(*cmd, (strlen(input) - strlen(*arg)), "%s", input);
    }
}

int fsh(char *cmd, char *arg, char *input, char *chemin, int dernier_exit,int ret)
{
    // printf("ret dans début fsh = %d\n",ret);
    if (strcmp(cmd, "exit") == 0)
    {
        dernier_exit = commande_exit(arg);
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
    // else if (cmd[0] == '\0' || cmd[0] == '\n' || cmd[0] == ' ' || cmd[0] == '\t') 
    // {
    //     ret=0;
    //     write(2,"\n",1);
    // }
    else
    {
        ret = cmd_extern(input);
        if (ret >= 1)
        {
            char *msg = malloc(MAX_INPUT);
            sprintf(msg, "Commande non reconnue : %s\n", cmd);
            write(2, msg, strlen(msg));
            if (msg != NULL)
            {
                free(msg);
            }
            ret=1;
            return ret;
        }
    }
    return ret;
}