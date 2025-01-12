#define _DEFAULT_SOURCE
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
#include "../../utils/gestionStruct.h"
#include "../../utils/freeStruct.h"
#include "../../utils/exit.h"
#include "../../utils/signaux.h"

int max = 0;
int nombre_fils = 0;
int retF = -233;

int compte_occ(char *chaine, char *sous_chaine)
{
    int res = 0;
    char *c = strstr(chaine, sous_chaine);
    while (c != NULL)
    {
        c += strlen(sous_chaine);
        res++;
        c = strstr(c, sous_chaine);
    }
    return res;
}

void eleverSlash(char *path)
{
    size_t len = strlen(path);
    if (len > 0 && path[len - 1] == '/')
    {
        path[len - 1] = '\0'; // Supprime le dernier '/'
    }
}

int nouveau_var_simple(char *ancienne, char *nouveau, cmd_simple *cmd)
{
    int k = 0;
    while (cmd->args[k] != NULL)
    {
        char *ancienne_cmd = strdup(cmd->args[k]);
        char *a_changer = strstr(ancienne_cmd, ancienne);
        if (a_changer == NULL)
        {
            k++;
        }
        else
        {
            int occ_ancienne = compte_occ(cmd->args[k], ancienne);
            int taille = strlen(cmd->args[k]) - occ_ancienne * strlen(ancienne) + occ_ancienne * strlen(nouveau) + 1;
            char *realloue = realloc(cmd->args[k], taille);
            if (realloue == NULL)
            {
                perror("Reallocation");
                free(ancienne_cmd);
                return 1;
            }
            cmd->args[k] = realloue;
            char *prefixe = ancienne_cmd;
            cmd->args[k][0] = '\0';
            while (a_changer != NULL)
            {
                int taille_prefixe = strlen(prefixe) - strlen(a_changer);
                if (taille_prefixe > 0)
                {
                    strncat(cmd->args[k], prefixe, taille_prefixe);
                }
                strcat(cmd->args[k], nouveau);
                prefixe = a_changer + strlen(ancienne);
                a_changer = strstr(prefixe, ancienne);
            }
            strcat(cmd->args[k], prefixe);
            strcat(cmd->args[k], "\0");
            k++;
        }
        if (ancienne_cmd != NULL)
            free(ancienne_cmd);
    }
    return 0;
}

int nouveau_var(char *ancienne, char *nouveau, commandeStruct *cmd)
{
    if (cmd == NULL)
        return 1;

    if (cmd->type == CMD_EXTERNE || cmd->type == CMD_INTERNE)
    {
        if (cmd->cmdSimple == NULL || cmd->cmdSimple->args == NULL)
            return 1;

        nouveau_var_simple(ancienne, nouveau, cmd->cmdSimple);
    }
    else if (cmd->type == FOR)
    {
        if (cmd->cmdFor == NULL)
            return 1;

        if (strcmp(cmd->cmdFor->rep, ancienne) == 0)
        {
            cmd->cmdFor->rep = realloc(cmd->cmdFor->rep, strlen(nouveau) + 1);
            sprintf(cmd->cmdFor->rep, "%s", nouveau);
        }
        nouveau_var(ancienne, nouveau, cmd->cmdFor->cmd);
    }
    else if (cmd->type == PIPE)
    {
        if (cmd->pipe == NULL || cmd->pipe->commandes == NULL)
            return 1;

        int l = 0;
        while (cmd->pipe->commandes[l] != NULL)
        {
            nouveau_var_simple(ancienne, nouveau, cmd->pipe->commandes[l]);
            l++;
        }
    }
    else if (cmd->type == CMD_STRUCT)
    {
        if (cmd->cmdsStruc == NULL)
            return 1;

        for (int i = 0; i < cmd->nbCommandes; i++)
        {
            nouveau_var(ancienne, nouveau, cmd->cmdsStruc[i]);
        }
    }
    else if (cmd->type == IF)
    {
        if (cmd->cmdIf == NULL)
            return 1;

        nouveau_var(ancienne, nouveau, cmd->cmdIf->test);
        nouveau_var(ancienne, nouveau, cmd->cmdIf->commandeIf);
        if (cmd->cmdIf->commandeElse != NULL)
        {
            nouveau_var(ancienne, nouveau, cmd->cmdIf->commandeElse);
        }
    }
    else if (cmd->type == REDIRECTION)
    {
        nouveau_var_simple(ancienne, nouveau, cmd->cmdSimple->red->cmd);

        if (cmd->cmdSimple->red->fichier != NULL && strstr(cmd->cmdSimple->red->fichier, "$") != NULL)
        {
            snprintf(cmd->cmdSimple->red->fichier, strlen(nouveau) + 1, "%s", nouveau);
        }
    }
    return 0;
}

int optionA(struct dirent *entry, cmdFor *cmdFor)
{
    return (rechercheDansArgs("-A", cmdFor->op) && entry->d_name[0] == '.' && entry->d_name[1] != '.' && entry->d_name[1] != '\0');
}

int arg_options(char **op, char *for_opt)
{
    for (int i = 0; op[i] != NULL; i++)
    {
        if (strcmp(op[i], for_opt) == 0)
        {
            return i + 1;
        }
    }
    return 0;
}

int option_e(struct dirent *entry, cmdFor *cmdFor)
{
    char *ext = cmdFor->op[arg_options(cmdFor->op, "-e")];
    char *basename = entry->d_name;
    char *dot = strrchr(basename, '.');

    if (dot != NULL && strcmp(dot + 1, ext) == 0 && basename[0] != '.') // pas un . a la 1ere pos
    {
        char *p = strstr(entry->d_name, ".");
        if (p != NULL)
        {
            if (entry->d_name[0] != p[0])
            {
                char *nom_sans_ext = malloc(strlen(entry->d_name) - strlen(p) + 1);
                memset(nom_sans_ext, 0, strlen(entry->d_name) - strlen(p) + 1);
                strncpy(nom_sans_ext, entry->d_name, strlen(entry->d_name) - strlen(p));
                sprintf(entry->d_name, "%s", nom_sans_ext);
                if (nom_sans_ext != NULL)
                    free(nom_sans_ext);
            }
        }
        return 1;
    }
    return 0;
}

int option_t(struct dirent *entry, cmdFor *cmd)
{
    int type = entry->d_type;
    int indice_op = arg_options(cmd->op, "-t");
    int for_type = -1;
    if (indice_op != 0)
    {
        if (strcmp(cmd->op[indice_op], "f") == 0)
        {
            for_type = 8;
        }
        else if (strcmp(cmd->op[indice_op], "d") == 0)
        {
            for_type = 4;
        }
        else if (strcmp(cmd->op[indice_op], "l") == 0)
        {
            for_type = 10;
        }
        else if (strcmp(cmd->op[indice_op], "p") == 0)
        {
            for_type = 1;
        }
        else
        {
            return -1;
        }
        return type == for_type;
    }
    else
    {
        return -1;
    }
}

void print_arg(commandeStruct *cmd)
{
    int k = 0;
    while (cmd->cmdSimple->args[k] != NULL)
    {
        printf("cmd->cmdSimple->args[%d] = %s\n", k, cmd->cmdSimple->args[k]);
        k++;
    }
}

int boucle_for(cmdFor *cmdFor);

int option_r(struct dirent *entry, cmdFor *cmd)
{
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
    {
        char path[PATH_MAX];
        if (cmd->rep[strlen(cmd->rep) - 1] != '/')
        {
            if (snprintf(path, sizeof(path), "%s/%s/", cmd->rep, entry->d_name) >= PATH_MAX)
            {
                perror("chemin trop long");
                return 1;
            }
        }
        else
        {
            if (snprintf(path, sizeof(path), "%s%s/", cmd->rep, entry->d_name) >= PATH_MAX)
            {
                perror("chemin trop long");
                return 1;
            }
        }
        // faire une copie pour pas modifier les champs de cmd
        cmdFor cmdCopie = *cmd;
        // copier le chemin
        cmdCopie.rep = strdup(path);
        if (cmdCopie.rep == NULL)
        {
            perror("copie du chemin");
            return 1;
        }
        int ret = boucle_for(&cmdCopie);
        if (ret > max)
        {
            max = ret;
        }
        free(cmdCopie.rep);
        if (ret == 1)
        {
            perror("fontion for dans -r");
            return 1;
        }
        return 0;
    }
    return 1;
}

int option_p(commandeStruct *cmd, int maxp)
{
    if (nombre_fils < maxp)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            return pid;
        }
        else if (pid == 0)
        {
            int r = fsh("", &dernier_exit, cmd);
            exit(r);
        }
        else
        {
            nombre_fils++;
            return -7;
        }
    }
    return -7;
}

int boucle_for(cmdFor *cmdFor)
{

    int ret = -255; 
    DIR *dir = opendir(cmdFor->rep);
    if (dir == NULL)
    {
        fprintf(stderr, "command_for_run: %s\n", cmdFor->rep);
        ret = 1;
        return ret;
    }
    struct dirent *entry;
    bool flag_p = false;
    int maxp = 0;
    pid_t pid_pere = getpid();
    while ((entry = readdir(dir)) != NULL)
    {

        if ((entry->d_name[0] != '.' || optionA(entry, cmdFor)))
        {

            if (rechercheDansArgs("-e", cmdFor->op))
            {
                if (!option_e(entry, cmdFor))
                {
                    continue;
                }
            }
            if (rechercheDansArgs("-r", cmdFor->op) && entry->d_type == DT_DIR)
            {
                int r = option_r(entry, cmdFor);
                if (r == 1)
                    break;
            }
            if (rechercheDansArgs("-t", cmdFor->op))
            {
                int res = option_t(entry, cmdFor);
                if (res == 0)
                {
                    continue;
                }
                if (res == -1)
                {
                    dernier_exit = 1;
                    closedir(dir);
                    return 2;
                }
            }
            int nbr_cmd = 0;
            while (cmdFor->cmd->cmdsStruc[nbr_cmd] != NULL)
            {

                char *inter = malloc(strlen(cmdFor->variable) + 2);
                strcpy(inter, "$");
                strcat(inter, cmdFor->variable);

                char *path = malloc(strlen(entry->d_name) + strlen(cmdFor->rep) + 2);
                if (path == NULL)
                {
                    closedir(dir);
                    return 1;
                }
                strcpy(path, cmdFor->rep);
                if (cmdFor->rep[strlen(cmdFor->rep) - 1] != '/')
                {
                    strcat(path, "/");
                }
                strcat(path, entry->d_name);
                strcat(path, "\0");
                int n = nouveau_var(inter, path, cmdFor->cmd->cmdsStruc[nbr_cmd]);
                if (n != 0)
                {
                    perror("problème dans nouveau");
                    free_for(cmdFor);
                    return 1;
                }
                if (flag_p || rechercheDansArgs("-p", cmdFor->op))
                {
                    if (flag_p == false)
                    {
                        flag_p = true;
                        int i = arg_options(cmdFor->op, "-p");
                        maxp = atoi(cmdFor->op[i]);
                    }
                    if (nombre_fils > maxp)
                    {
                        printf("Trop d'itérations en parallèle !\n");
                        break;
                    }
                    while (nombre_fils > maxp - 1)
                    {
                        int status;
                        pid_t pid = wait(&status);
                        if (pid > 0)
                        {
                            nombre_fils--;
                            if (WIFEXITED(status))
                            {
                                retF = WEXITSTATUS(status);
                                if (retF > max)
                                {
                                    max = ret;
                                }
                                if (retF == -255)
                                {
                                    max = -255;
                                }
                            }
                        }
                    }
                    int g;
                    if (pid_pere == getpid())
                    {
                        g = option_p(cmdFor->cmd->cmdsStruc[nbr_cmd], maxp);
                    }
                    if (g == -7)
                    {
                        if (nombre_fils > maxp)
                        {
                            printf("Trop d'itérations en parallèle !\n");
                            break;
                        }
                        while (nombre_fils > maxp - 1)
                        {
                            int status;
                            pid_t pid = wait(&status);
                            if (pid > 0)
                            {
                                nombre_fils--;
                                if (WIFEXITED(status))
                                {
                                    ret = WEXITSTATUS(status);
                                    if (retF > max)
                                    {
                                        max = ret;
                                    }
                                    if (retF == -255)
                                    {
                                        max = -255;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    ret = fsh("", &dernier_exit, cmdFor->cmd->cmdsStruc[nbr_cmd]);
                }
                if (ret == -255)
                {
                    max = -255;
                }
                if (ret > max)
                {
                    max = ret;
                }
                if (cmdFor->cmd->cmdsStruc[nbr_cmd] == NULL)
                {
                    perror("pb ds le changement de var");
                    free_for(cmdFor);
                    return 1;
                }
                char *ancienne = malloc(strlen(entry->d_name) + strlen(cmdFor->rep) + 2);
                strcpy(ancienne, cmdFor->rep);
                if (cmdFor->rep[strlen(cmdFor->rep) - 1] != '/')
                {
                    strcat(ancienne, "/");
                }
                strcat(ancienne, entry->d_name);
                char *dollar = malloc(strlen(cmdFor->variable) + 2);
                strcpy(dollar, "$");
                strcat(dollar, cmdFor->variable);
                strcat(path, "\0");
                n = nouveau_var(ancienne, dollar, cmdFor->cmd->cmdsStruc[nbr_cmd]);
                if (n != 0)
                {
                    perror("problème dans le 2ème appel de nv");
                    free_for(cmdFor);
                    return 1;
                }

                nbr_cmd = nbr_cmd + 1;

                if (dollar != NULL)
                    free(dollar);
                if (ancienne != NULL)
                    free(ancienne);
                if (path != NULL)
                    free(path);
                if (inter != NULL)
                    free(inter);
            }
        }
    }
    if (flag_p == true)
    {
        if (nombre_fils > maxp)
        {
            printf("Trop d'itérations en parallèle !\n");
        }
        while (nombre_fils > 0)
        {
            int status;
            pid_t pid = wait(&status);
            if (pid > 0)
            {
                nombre_fils--;
                if (WIFEXITED(status))
                {
                    retF = WEXITSTATUS(status);
                    if (retF > max)
                    {
                        max = retF;
                    }
                    if (retF == -255)
                    {
                        max = -255;
                    }
                }
            }
        }
    }

    closedir(dir);
    return ret;
}
