#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include "../../utils/redirection.h"
#include "../../utils/gestion.h"
#include "../../utils/extern.h"
#include "../../utils/ftype.h"
#include <linux/limits.h>
#include "../../utils/commande.h"
#include "../../utils/freeStruct.h"
#include "../../utils/exit.h"
#define ARG_MAX 512

void nouveau(char *ancienne, char *nouveau, commandeStruct *cmd)
{
    if (cmd->type == CMD_EXTERNE || CMD_INTERNE)
    {
        int k = 0;
        while (cmd->cmdSimple->args[k] != NULL)
        {
            char *a_changer = strstr(cmd->cmdSimple->args[k], ancienne);
            if (a_changer == NULL)
            {
                k++;
            }
            else
            {
                // printf("a_changer = %s\n",a_changer);
                int len_prefixe = strlen(cmd->cmdSimple->args[k]) - strlen(a_changer);
                // printf("len_prefixe = %d\n",len_prefixe);
                // char *postfixe  = cmd->cmdSimple->args[k]+strlen(ancienne);
                char *postfixe;
                if (strlen(a_changer) + strlen(ancienne) == strlen(cmd->cmdSimple->args[k]))
                {
                    printf("a_changer = %s\n", a_changer);
                    postfixe = cmd->cmdSimple->args[k] + strlen(a_changer) + strlen(ancienne);
                }
                else
                {
                    postfixe = cmd->cmdSimple->args[k] + strlen(ancienne);
                }

                if (strcmp(postfixe, "") == 0)
                {
                    postfixe = NULL;
                }
                char *prefixe[len_prefixe + 1];
                int len_final = strlen(nouveau);
                if (len_prefixe > 0)
                {
                    strncpy(prefixe, cmd->cmdSimple->args[k], len_prefixe);
                    len_final += len_prefixe;
                }
                cmd->cmdSimple->args[k] = NULL;
                int len_postfixe = 0;
                if (postfixe != NULL)
                {
                    len_postfixe = strlen(postfixe);
                    // printf("postfixe = %s\n",postfixe);
                    len_final += len_postfixe;
                    // printf("nouveau = %s\n",nouveau);
                }
                char *realloue = realloc(cmd->cmdSimple->args[k], len_final + 2);
                cmd->cmdSimple->args[k] = realloue;

                if (len_prefixe > 0)
                {
                    // printf("a_changer = %s\n",a_changer);
                    printf("postfixe = %s\n", postfixe);
                    printf("prefixe = %s\n", prefixe);
                    printf("nouveau =%s\n", nouveau);
                    printf("ancien = %s\n", ancienne);
                    // printf("cmdSimple->args[%d] = %s\n",k,cmd->cmdSimple->args[k]);
                    strcpy(cmd->cmdSimple->args[k], prefixe);
                    strcat(cmd->cmdSimple->args[k], nouveau);
                }
                else
                {
                    strcpy(cmd->cmdSimple->args[k], nouveau);
                    // printf("cmd->cmdSimple->args[%d] = %s\n",k,cmd->cmdSimple->args[k]);
                }
                if (len_postfixe > 0)
                {
                    strcat(cmd->cmdSimple->args[k], postfixe);
                }
                k++;
            }
        }
    }
}

void restaurer_var(char *ancienne, char *nouveau, commandeStruct *cmd)
{
    switch (cmd->type)
    {
    case CMD_EXTERNE:
        int k = 0;
        while (cmd->cmdSimple->args[k] != NULL)
        {
            char *var_str = strstr(cmd->cmdSimple->args[k], ancienne);
            if (strcmp(cmd->cmdSimple->args[k], ancienne) == 0)
            {
                cmd->cmdSimple->args[k] = NULL;
                cmd->cmdSimple->args[k] = malloc(strlen(nouveau) + 2);
                strcpy(cmd->cmdSimple->args[k], "$");
                strcat(cmd->cmdSimple->args[k], nouveau);
            }
            else if (var_str != NULL)
            { // TODO FAIRE LA MM PR LE RESTE
                char *reste = strstr(cmd->cmdSimple->args[k], ancienne);
                reste += strlen(ancienne);
                char *realloue = realloc(cmd->cmdSimple->args[k], strlen(nouveau) + 2); // PR 0 ET $
                if (realloue == NULL)
                {
                    perror("realloc");
                    return;
                }
                cmd->cmdSimple->args[k] = realloue;
                strcpy(cmd->cmdSimple->args[k], "$");
                strcat(cmd->cmdSimple->args[k], nouveau);
                strcat(cmd->cmdSimple->args[k], reste);
            }
            k++;
        }
        break;
    default:
        break;
    }
}

int optionA(struct dirent *entry, cmdFor *cmdFor)
{
    return (rechercheDansArgs("-A", cmdFor->op) && entry->d_name[0] == '.' && entry->d_name[1] != '.' && entry->d_name[1] != '\0');
}

int pos_arg_options(char **op)
{
    for (int i = 0; op[i] != NULL; i++)
    {
        if (strcmp(op[i], "-e") == 0)
        {
            return i + 1;
        }
    }
    return 0;
}

int option_e(struct dirent *entry, cmdFor *cmdFor)
{
    if (rechercheDansArgs("-e", cmdFor->op))
    {
        char *ext = cmdFor->op[pos_arg_options(cmdFor->op)];
        char *basename = entry->d_name;
        if (strlen(ext) > 0 && basename[strlen(basename) - strlen(ext) - 1] == '.' && basename[0] != '.')
        {
            // printf("basename[strlen(basename) - strlen(ext) - 1] = [%c]\n", basename[strlen(basename) - strlen(ext) - 1]);
            // printf("Nom du fichier: [%s]\n", basename);
            // printf("extension [%s]\n", ext);
            return 1;
        }
    }
    return 0;
}
// TODO changer la fct en int car void pas ouf si pb de malloc realloc tt ça faut renvoyer un int qui dis que y'a un pb

// for i in rep { ls -l $i ; echo $i ; }
// ! for i in . -e txt { ftype $i }

// TODO Si ca ce passe mal ft faire un truc
int boucle_for(cmdFor *cmdFor)
{
    // int dernier_exit = 0; // ! pas besoin dernier exit est une var globale dans exit.h
    int ret = 0; // TODO A CHANGER;
    DIR *dir = opendir(cmdFor->rep);
    if (dir == NULL)
    {
        fprintf(stderr, "command_for_run: %s\n", cmdFor->rep);
        ret = 1;
        return ret;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // printf("dans le while\n");
        if ((entry->d_name[0] != '.' || optionA(entry, cmdFor)))
        {
            if (!option_e(entry, cmdFor))
            {
                continue;
            }
            // if l'option est activé et que le fichier contient l'extenstion alors affiché sinon ca dégage
            //  printf("entry->d_name = [%s] \n", entry->d_name);
            //? ici
            int nbr_cmd = 0;
            while (cmdFor->cmd[nbr_cmd] != NULL)
            {
                char *inter = malloc(strlen(cmdFor->variable) + 2); // ? CA C PR AVOIR LE BON NOM DE VARIABLE +2 pr $ et le char 0
                strcpy(inter, "$");
                strcat(inter, cmdFor->variable);
                // printf("inter = %s\n",inter);

                char *path = malloc(strlen(entry->d_name) + strlen(cmdFor->rep) + 2); // +2 pr / et '\0'
                if (path == NULL)
                {
                    return 1;
                }
                strcpy(path, cmdFor->rep);
                strcat(path, "/");
                strcat(path, entry->d_name);
                // printf("path = %s\n",path);

                nouveau(inter, path, cmdFor->cmd[nbr_cmd]);

                // change_var(entry->d_name,cmdFor->variable, cmdFor->cmd[nbr_cmd],cmdFor->rep);
                char *chemin = malloc(PATH_MAX);
                if (getcwd(chemin, PATH_MAX) == NULL)
                {
                    perror("getcwd");
                    free(chemin);
                    return 1;
                }

                ret = fsh(chemin, &dernier_exit, cmdFor->cmd[nbr_cmd]);
                if (cmdFor->cmd[nbr_cmd] == NULL)
                {
                    perror("pb ds le changement de var");
                    free_for(cmdFor);
                    return 1;
                }

                char *ancienne = malloc(strlen(entry->d_name) + strlen(cmdFor->rep) + 2);
                strcpy(ancienne, cmdFor->rep);
                strcat(ancienne, "/");
                strcat(ancienne, entry->d_name);
                restaurer_var(ancienne, cmdFor->variable, cmdFor->cmd[nbr_cmd]);
                if (cmdFor->cmd[nbr_cmd] == NULL)
                {
                    perror("pb ds le changement de var");
                    free_for(cmdFor);
                    return 1;
                }
                if (inter != NULL)
                    free(inter);
                if (path != NULL)
                    free(path);
                if (ancienne != NULL)
                    free(ancienne);
                if (chemin != NULL)
                    free(chemin);
                nbr_cmd = nbr_cmd + 1;
            }
        }
    }
    closedir(dir);

    return ret;
}