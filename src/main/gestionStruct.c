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
#include "../../utils/freeStruct.h"
#define ARG_MAX 512
// #include <stdbool.h>

size_t tailleArgs(char **args)
{
    size_t taille = 0;
    while (args[taille] != NULL)
    {
        taille++;
    }
    return taille + 1; // inclure la derniere case NULL
}
int arg_cmdsimple(char **args, char **commande, int i, int j)
{
    for (int h = 0; h < (i - j); h++)
    {
        commande[h] = strdup(args[h + j]);
        if (commande[h] == NULL)
        {
            return 1;
        }
    }
    commande[i - j] = NULL; // pour le dernier élementp
    return 0;
}

commandeStruct *remplissage_cmdStruct(Type type, cmd_simple *cmdSimple, cmd_pipe *pipestruct, cmdIf *cmdIfStruct, cmdFor *cmdForStruct, cmd_redirection *cmdredirection, int nbcommandes, commandeStruct *cmd)
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

cmd_simple *remplissage_cmdSimple(char **args)
{
    cmd_simple *cmd = malloc(sizeof(cmd_simple));
    // printf("dans remplissage cmd simple \n");

    if (cmd == NULL)
    {
        perror("malloc CommandSimple");
        return NULL;
    }
    int nbargs = 0;
    while (args[nbargs])
    {
        // printf("args[%d] = %s\n",nbargs,args[nbargs]);
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
        // printf("args[%d] = %s\n",i,cmd->args[i]);
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
    if (arg_cmdsimple(args, commande, taille - 3, 0) == 1)
    {
        perror("strdup cmd_redirection");
        free_redirection(cmd);
        return NULL;
    }
    // TODO : erreur sur la pos du separateur
    size_t pos_sep = tailleArgs(commande) -1;
    cmd->cmd = remplissage_cmdSimple(commande);
    if (cmd->cmd == NULL)
    {
        perror("remplissage cmd simple");
        free_redirection(cmd);
        return NULL;
    }
    if (strstr(args[pos_sep], ">") != NULL)
    {
        cmd->fichier = args[taille - 2]; // l'avant dernier élement vu que le dernier est NULL
        cmd->separateur = ">";
    }
    else if (strstr(args[pos_sep], " >> ") != NULL)
    {
        cmd->fichier = args[taille - 2];
        cmd->separateur = ">>";
    }
    else if (strstr(args[pos_sep], " 2> ") != NULL)
    {
        cmd->fichier = args[taille - 2];
        cmd->separateur = "2>";
    }
    else if (strstr(args[pos_sep], " >| ") != NULL)
    {
        cmd->fichier = args[taille - 2];
        cmd->separateur = " >| ";
    }
    else if (strstr(args[pos_sep], "2>|") != NULL)
    {
        cmd->fichier = args[taille - 1];
        cmd->separateur = "2>|";
    }
    else if (strstr(args[pos_sep], " 2>> ") != NULL)
    {
        cmd->fichier = args[taille - 2];
        cmd->separateur = "2>>";
    }
    else if (strstr(args[pos_sep], "<") != NULL)
    {
        cmd->fichier = args[taille - 2];
        cmd->separateur = "<";
    }else{
        perror("Erreur de syntaxe");
        free_redirection(cmd);
        return NULL;
    }
    return cmd;
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
            if (arg_cmdsimple(args, commande, i, j) == 1)
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
            if (arg_cmdsimple(args, commande, i, j) == 1)
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



// // type
// //   char* rep;
// //   char variable ; 
//  //int nbCommandes;
// commandeStruct** cmd;
// //  op

cmdFor* make_for(char ** args){
    // cmdFor *cmdFor = malloc(sizeof(*cmdFor));

    cmdFor *cmdFor = malloc(sizeof(*cmdFor));
    if (cmdFor == NULL){
        perror("problème d'allocation de mémoire pour for");
        return NULL;
    }
    // printf("Adresse de cmdFor %p\n", cmdFor);
    // TODO j'ai commenté
    if (tailleArgs(args) < 8)
    {
        perror("problème de syntaxe");
        printf("la taille de l'argument = %ld\n", tailleArgs(args));
        for (size_t i = 0; i < tailleArgs(args); i++)
        {
            printf("%s\n", args[i]);
        }
        // free_for(cmdFor);
        return NULL;
    }
    // memset(cmdFor,0,sizeof(cmdFor));
    cmdFor->rep = NULL;
    cmdFor->op = NULL;
    // cmdFor->op[0] = NULL;
    cmdFor->variable = NULL;
    cmdFor->cmd = NULL;

    //* --------- option----------
    // cmdFor->op = NULL;
    // cmdFor->nbCommandes = malloc(sizeof(int));
    // if (cmdFor->nbCommandes == NULL){
    //     free_for(cmdFor);
    // }
    // cmdFor->nbCommandes = 0;
    // ? -------- Type ---------
    cmdFor->type = FOR;

    // * ------------------ variable ---------------
    if (strlen(args[1]) != 1)
    {
        perror("Erreur de syntaxe, la variabme doit contenir un seul caractère");
        // free_for(cmdFor);
        return NULL;
    }

    size_t taille = tailleArgs(args);
    // for (size_t i = 0 ; i <taille; i++){
    //     printf("args[%ld] = %s \n",i,args[i]);
    //     printf("taille %d \n",sizeof(args[i]));

    // }

    cmdFor->variable = strdup(args[1]);
    // perror("strdup variable");
    if (cmdFor->variable == NULL)
    {
        perror("erreur de duuuup");
        free_for(cmdFor);
        return NULL;
    }

    cmdFor->rep = strdup(args[3]);
    // if (cmdFor->rep == NULL) {
    //     perror("erreur de duuuuuuuup");
    //     // free_for(cmdFor);
    //     return NULL;
    // }
    cmdFor->op = malloc(ARG_MAX*sizeof(char));
    if (cmdFor->op == NULL) {
        perror("aie aie aie");
        free_for(cmdFor);
        return NULL;
    }
    // ? ----------------- option-----------
    int i = 4;
    int j = 0;
    // bool flag = false;
    while (strcmp(args[i], "{") != 0)
    {
        // printf("chui dans le while \n");
        if (strcmp(args[i], "-A") == 0 || strcmp(args[i], "-r") == 0)
        {
            cmdFor->op[j] = args[i]; // TODO utiliser strdup
            i++;
            j++;
            // flag = true;
        }
        else if (strcmp(args[i], "-e") == 0 || strcmp(args[i], "-t") == 0 || strcmp(args[i], "-p") == 0)
        {
            if (args[i + 1][0] != '-')
            {
                cmdFor->op[j] = args[i]; // TODO a changer
                i = i + 2;
                j++;
                // flag = true;
            }
            else
            {
                perror("il manque un argument");
                free_for(cmdFor);
                return NULL;
            }
        }
        // if (flag == false){
        //     cmdFor->op[0] = NULL;
        // }
        // TODO j'ai commenté
        //  else{
        //      perror("ce n'est pas un argument valide");
        //      return NULL;
        //  }
    }
    cmdFor->op[j] = NULL;
    if (strcmp(args[i], "{") == 0)
    {
        // printf("args[%d] = %s\n",i,args[i]);
        // printf("chui laaaa\n");
        i++; // pour sauter l'{
    }
    
    // printf("args[%d] = %s\n",i,args[i]);
    cmdFor->cmd = malloc(sizeof(commandeStruct));
    if (cmdFor->cmd == NULL){
        perror("pb d'alloc de sous cmd de for");
        free_for(cmdFor);
        //TODO APPELER LES FREE
        return NULL;
    }

    char *tab[ARG_MAX];
    unsigned int k = 0;
    while (args[i] != NULL && i < taille && strcmp(args[i], "}") != 0)
    { // TODO ATTENTION PR LES CMD PLUS COMPLEXE LE STRCMP } PAS OUF
        tab[k] = args[i];
        k = k + 1;
        i = i + 1;
    }
    tab[k]= NULL;
    // char *inter=NULL;
    // inter=strdup(tab[0]);//TODO A CHANGER le probleme c que gestion prends un string au lieu d'un tableau du coup chui obligé de recoller tout le monde 
    // char *c = " ";
    // size_t len = strlen(inter) + strlen(c) + strlen(tab[1]) + 1;
    // char *temp = realloc(inter,len);
    // inter = temp;
    // strcat(inter,c);
    // strcat(inter,tab[1]);
    // if (tab[2] != NULL){
    //     len = strlen(inter) + strlen(c) + strlen(tab[2]) + 1;
    //     temp = realloc(inter,len);
    //     inter = temp;
    //     strcat(inter,c);
    //     strcat(inter,tab[2]);
    // }
    // printf("inter = %s",inter);
    // // for (int i = 0; i <k;i++){
    // //     printf(tab[i]);
    // //     printf("\n");
    // // }
    // // printf("inter = %s\n",inter);
    // // printf("dans make for var = %s\n",cmdFor->variable);
    // memset(cmdFor->cmd,0,ARG_MAX);
    cmdFor->cmd[0] = malloc(sizeof(commandeStruct));
    cmdFor->cmd[1] = NULL; // TODO A CHANGER si j'ai plusieurs commande ça ne marche pas hein
    // gestion_cmd(inter,cmdFor->cmd[0]);
    gestion_cmd(tab,cmdFor->cmd[0]);
    // if (cmdFor->cmd==NULL){
    //     printf("chui BIEN NULLLE\n");
    //     return NULL;
    // }else{
    //     printf("chui pas null le bebe cdm du for\n");
    //     printf("le type dans make for du bébé cmd de for = %d\n",cmdFor->cmd[0]->type);
    //     // fflush(NULL);
    // }


    // boucle_for(cmdFor);
    return cmdFor;
}
