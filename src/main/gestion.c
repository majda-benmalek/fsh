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
        // printf("espace + pas d'arg\n");
        snprintf(cmd, strlen(input), "%s", input);
    }
    if (espace == NULL)
    {
        snprintf(cmd, strlen(input) + 2, "%s", input); //\0
    }
    if (strlen(arg) >= 1)
    {
        snprintf(cmd, (strlen(input) - strlen(arg)), "%s", input);
    }
}

char **ges2(char *input)
{
    char **arg = malloc(ARG_MAX * sizeof(char *));
    if (arg == NULL)
    {
        write(2, "Erreur d'allocation de mémoire\n", strlen("Erreur d'allocation de mémoire\n"));
        return NULL;
    }

    char *copy = strdup(input);
    if (copy == NULL)
    {
        write(2, "Erreur d'allocation de mémoire\n", strlen("Erreur d'allocation de mémoire\n"));
        free(arg);
        return NULL;
    }

    char *token = strtok(copy, " ");

    int count = 0;
    while (token != NULL)
    {
        arg[count] = strdup(token);
        if (arg[count] == NULL)
        {
            write(2, "Erreur d'allocation de mémoire\n", strlen("Erreur d'allocation de mémoire\n"));
            free(copy);
            for (int i = 0; i < count; i++)
            {
                free(arg[i]);
            }
            free(arg);
            return NULL;
        }
        count++;
        token = strtok(NULL, " ");
    }
    arg[count] = NULL;
    //affiché arg
    for (int i = 0; i < count; i++)
    {
        printf("arg[%d] : %s\n", i, arg[i]);
    }

    return arg;
}

int fsh(char *cmd, char *arg, char *input, char *chemin, int dernier_exit)
{
    int ret = 0;
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
        if (arg != NULL)
        {
            free(arg);
        }
        if (cmd != NULL)
        {
            free(cmd);
        }
        exit(dernier_exit);
    }
    //* Commande cd
    else if (strcmp(cmd, "cd") == 0)
    {
        ret = cd_commande(arg);
        if (ret >= 1)
        {
            perror("cd");
        }
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
            ret = 1;
            return ret;
        }
    }
    return ret;
}

// int fsh(char **arg, char *chemin, int dernier_exit)
// {
//     int ret=0;
//     if (strcmp(arg[0], "exit") == 0)
//     {
//         dernier_exit = commande_exit(*arg);
//         if (arg != NULL)
//         {
//             free(arg);
//         }
//         if (chemin != NULL)
//         {
//             free(chemin);
//         }
//         exit(dernier_exit);
//     }
//     //* Commande cd
//     else if (strcmp(arg[0], "cd") == 0)
//     {
//         printf("arg[1]: %s\n",arg[1]);
//         ret = cd_commande(arg[1]);
//         if(ret>=1){
//             perror("cd");
//         }
//         if (getcwd(chemin, PATH_MAX) == NULL)
//         {
//             perror("getcwd");
//             return 1;
//         }
//         return ret;
//     }
//     //* Commande pwd
//     else if (strcmp(arg[0], "pwd") == 0)
//     {
//         ret = pwd();
//     }
//     //* Redirection > et >>
//     else if (strstr(*arg, ">>") || strstr(*arg, ">"))
//     {
//         ret = redirection(*arg);
//         if (ret != 0)
//         {
//            perror("Redirection");
//            return ret;
//         };
//     }
//     else if (strcmp(arg[0], "ftype") == 0)
//     {
//         ret = ftype(arg[1]);
//         if (ret > 0)
//         {
//             perror("ftype");
//             return ret;
//         }
//     }
//     else if (strstr(arg[0], "for"))
//     {
//         ret = boucle_for(arg[0]);
//         if (ret != 0)
//         {
//             perror("boucle_for");
//             return ret;
//         };
//     }
//     else
//     {
//         ret = cmd_extern(arg[0]);
//         if (ret >= 1)
//         {
//             char *msg = malloc(MAX_INPUT);
//             sprintf(msg, "Commande non reconnue : %s\n", arg[0]);
//             write(2, msg, strlen(msg));
//             if (msg != NULL)
//             {
//                 free(msg);
//             }
//             ret=1;
//             return ret;
//         }
//     }
//     return ret;
// }