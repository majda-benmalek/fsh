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
#include "../../utils/freeStruct.h"
#include "../../utils/exit.h"
#define ARG_MAX 512

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

//TODO A CHANGER PR QUE SOIT PR TT LES TYPES

// //  CMD_EXTERNE,
// //  CMD_INTERNE,
//   REDIRECTION,
//  // CMD_STRUCT,
// //  FOR,
//   IF,
//   //PIPE

int nouveau_var(char *ancienne, char *nouveau, commandeStruct *cmd)
{
    if (cmd->type == CMD_EXTERNE || cmd->type == CMD_INTERNE)
    {
        // printf("chui dans nouveau\n");
        // print_arg(cmd->cmdSimple->args);
        int k = 0;
        //  if (cmd->cmdSimple->args[k] != NULL){
        //         printf("cmd->cmdSimple->args[%d] = %s\n",k,cmd->cmdSimple->args[k]);
        // }
        // else{
        //     printf("chui null\n");
        // }
        while (cmd->cmdSimple->args[k] != NULL)
        {
            // if (cmd->cmdSimple->args[k] != NULL){
            //     printf("cmd->cmdSimple->args[%d] = %s\n",k,cmd->cmdSimple->args[k]);
            // }
            // else{
            //     printf("chui null\n");
            //     return 1;
            // }
            char *ancienne_cmd = strdup(cmd->cmdSimple->args[k]);
            char *a_changer = strstr(ancienne_cmd, ancienne);
            if (a_changer == NULL)
            {
                k++;
            }
            else
            {
                int occ_ancienne = compte_occ(cmd->cmdSimple->args[k], ancienne);
                int taille = strlen(cmd->cmdSimple->args[k]) - occ_ancienne * strlen(ancienne) + occ_ancienne * strlen(nouveau) + 1;
                char *realloue = realloc(cmd->cmdSimple->args[k], taille); 
                if (realloue == NULL)
                {
                    perror("Reallocation");
                    return 1;
                }
                cmd->cmdSimple->args[k] = realloue;
                char *prefixe = ancienne_cmd;
                cmd->cmdSimple->args[k][0] = '\0'; // pour pas qu'il soit à null
                while (a_changer != NULL)
                {
                    int taille_prefixe = strlen(prefixe) - strlen(a_changer);
                    if (taille_prefixe > 0)
                    {
                        strncat(cmd->cmdSimple->args[k], prefixe, taille_prefixe);
                    }
                    strcat(cmd->cmdSimple->args[k], nouveau);
                    prefixe = a_changer + strlen(ancienne);
                    a_changer = strstr(prefixe, ancienne);
                }
                strcat(cmd->cmdSimple->args[k], prefixe);
                strcat(cmd->cmdSimple->args[k],"\0");
                // printf("dans nouveau\n cmd->cmdSimple->args[%d] = %s\n",k,cmd->cmdSimple->args[k]);
                k++;
            }
            if (ancienne_cmd != NULL)
                free(ancienne_cmd);
        }
    }
    else if (cmd->type == FOR){
        // printf("chui bien dans le else if\n et ancienne = %s et nouveau = %s\n",ancienne,nouveau);
        if (strcmp(cmd->cmdFor->rep,ancienne) == 0){
            cmd->cmdFor->rep = realloc(cmd->cmdFor->rep ,strlen(nouveau) + 1);
            sprintf(cmd->cmdFor->rep,"%s",nouveau);
            // free(cmd->cmdFor->rep);
            // cmd->cmdFor->rep=malloc(strlen(nouveau)+1);
            // cmd->cmdFor->rep=nouveau;
            // printf("repertoire = %s\n",cmd->cmdFor->rep);
        }
        nouveau_var(ancienne,nouveau,cmd->cmdFor->cmd);
    }
    else if (cmd->type == PIPE){
        // printf("chui dans pipe\n");
        int l = 0;
        commandeStruct *inter_type = malloc(sizeof(commandeStruct));
        while(cmd->pipe->commandes[l] != NULL){
            inter_type->cmdSimple=cmd->cmdIf->test->commandes[l];
            inter_type->type=cmd->cmdIf->test->commandes[l]->type;
            nouveau_var(ancienne,nouveau,inter_type);
            l++;
        }
        // for (int i = 0; i < cmd->pipe->nbCommandes; i++){
        // }
    }
    else if (cmd->type == CMD_STRUCT){
        for (int i = 0; i< cmd->nbCommandes ; i++){
            nouveau_var(ancienne,nouveau,cmd->cmdsStruc[i]);
        }
    }
    else if(cmd->type == IF){
        // printf("je rentre dans if\n");
        int i = 0;
        commandeStruct *inter=malloc(sizeof(commandeStruct));
        while (cmd->cmdIf->test->commandes[i]!= NULL){
            printf("chui dans le while \n");
            inter->cmdSimple=cmd->cmdIf->test->commandes[i];
            inter->type=cmd->cmdIf->test->commandes[i]->type;
            // nouveau_var(ancienne,nouveau,cmd->cmdIf->test->commandes[i]);
            nouveau_var(ancienne,nouveau,inter);
            i++;
        }
        nouveau_var(ancienne,nouveau,cmd->cmdIf->commandeIf);
        if (cmd->cmdIf->commandeElse!= NULL){
            // printf("je rentre dans le else\n");
            nouveau_var(ancienne,nouveau,cmd->cmdIf->commandeElse);
        }
        // int i = 0;
        // while (cmd->cmdIf->test->commandes[i]!= NULL){
        //     // printf("chui dans le while \n");
        //     nouveau_var(ancienne,nouveau,cmd->cmdIf->test->commandes[i]);
        //     i++;
        // }
         // for (int i = 0 ; i < cmd->cmdIf->test->nbCommandes ; i++){
        // }
    }
    // printf("chui rien\n");
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
        // printf("Nom du fichier: [%s]\n", basename);
        // printf("Extension: [%s]\n", ext);
        // printf("extension du fichier dot+1 : [%s]\n",dot+1);
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

void print_arg(commandeStruct *cmd){
    int k = 0;
    while (cmd->cmdSimple->args[k] != NULL)
    {
        printf("cmd->cmdSimple->args[%d] = %s\n",k,cmd->cmdSimple->args[k]);
        k++;
    }
}


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
        free(cmdCopie.rep);
        if (ret == 1)
        {
            perror("fontion for dans -r");
            return 1;
        }
        return 0;
    }
    return 0;
}




// TODO ERREUR DE SYNTAXE CODE ERREUR = 2
//  TODO Si ca ce passe mal ft faire un truc
// TODO JE FERME PAS LE REP ?
int boucle_for(cmdFor *cmdFor)
{
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
        if ((entry->d_name[0] != '.' || optionA(entry, cmdFor)))
        {

            if (rechercheDansArgs("-e", cmdFor->op))
            {
                if (!option_e(entry, cmdFor))
                {
                    continue;
                }
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
                    return 1;
                }
            }

            if (rechercheDansArgs("-r", cmdFor->op) && entry->d_type == DT_DIR)
            {
                ret = option_r(entry, cmdFor);
                if (ret == 1)
                    break;
                continue;
            }

            int nbr_cmd = 0;
            while (cmdFor->cmd->cmdsStruc[nbr_cmd] != NULL)
            {
                char *inter = malloc(strlen(cmdFor->variable) + 2); // ? CA C PR AVOIR LE BON NOM DE VARIABLE +2 pr $ et le char 0
                strcpy(inter, "$");
                strcat(inter, cmdFor->variable);

                char *path = malloc(strlen(entry->d_name) + strlen(cmdFor->rep) + 2); // +2 pr / et '\0'
                if (path == NULL)
                {
                    return 1;
                }
                strcpy(path, cmdFor->rep);
                if (cmdFor->rep[strlen(cmdFor->rep) - 1] != '/')
                {
                    strcat(path, "/");
                }
                strcat(path, entry->d_name);
                strcat(path,"\0");
                // printf("path = %s\n",path);
                int n = nouveau_var(inter, path, cmdFor->cmd->cmdsStruc[nbr_cmd]);
                if (n!= 0){
                    perror("problème dans nouveau");
                    free_for(cmdFor);
                    return 1;
                }
                // print_arg(cmdFor->cmd->cmdsStruc[nbr_cmd]);
                ret = fsh("", &dernier_exit, cmdFor->cmd->cmdsStruc[nbr_cmd]);
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
                char *dollar = malloc(strlen(cmdFor->variable) + 2); // ? CA C PR AVOIR LE BON NOM DE VARIABLE +2 pr $ et le char 0
                strcpy(dollar, "$");
                strcat(dollar, cmdFor->variable);
                strcat(path,"\0");
                // printf("ancienne = %s\n",ancienne);
                n = nouveau_var(ancienne, dollar, cmdFor->cmd->cmdsStruc[nbr_cmd]);
                if (n != 0){
                    perror("problème dans le 2ème appel de nv");
                    free_for(cmdFor);
                    return 1;
                }
                // if (cmdFor->cmd->cmdsStruc[nbr_cmd] == NULL)
                // {
                //     perror("pb ds le changement de var");
                //     free_for(cmdFor);
                //     // closedir(dir);
                //     return 1;
                // }
                // printf("-----------\n");
                nbr_cmd = nbr_cmd + 1;
                // printf("nbr_cmd = %d\n",nbr_cmd);
                // printf("-----------\n");
                // if (dollar != NULL)
                //     free(dollar);
                // if (ancienne != NULL)
                //     free(ancienne);
                // if (path != NULL)
                //     free(path);
                // if (inter != NULL)
                //     free(inter);
            }
        }
    }
    closedir(dir);

    return ret;
}