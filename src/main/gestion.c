#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
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
#include "../../utils/commande.h"
#include "../../utils/gestionStruct.h"
#include "../../utils/pipe.h"
#define ARG_MAX 512

int rechercheDansArgs(char* tofind, char** args){
    for(int i=0; i<tailleArgs(args)-1;i++){
        if(strcmp(args[i],tofind)==0){
            return 1;
        }
    }
    return 0;
}

// TODO : changé input en args dans main et gestion prends en args
void gestion_cmd(char *input, commandeStruct *cmdstruct)
{

    if (!cmdstruct)
    {
        perror("Erreur Structure");
        return;
    }
    // Transforme en input en tableau d'arguments
    char *args[ARG_MAX] = {NULL};
    int nb_args = 0;
    char *token = strtok(input, " \t"); // pour gerer le cas ou l'utilisateur separe les arguments avec tab
    while (token && nb_args < ARG_MAX - 1)
    {
        args[nb_args++] = token;
        token = strtok(NULL, " \t");
    }
    args[nb_args] = NULL;
    // REDIRECTION
    if (strstr(input, ">") != NULL || strstr(input, "<") != NULL || strstr(input, ">>") != NULL || strstr(input, ">|") != NULL || strstr(input, "2>") != NULL || strstr(input, "2>>") != NULL || strstr(input, "2>|") != NULL)
    {
        cmdstruct->cmdRed = remplissageCmdRedirection(args);
        cmdstruct->type = REDIRECTION;
        if (cmdstruct->cmdRed == NULL)
        {
            perror("Erreur remplissage redirection");
        }
    }
    else if (rechercheDansArgs("|",args))
    {
        cmdstruct->pipe = remplissageCmdPipe(args);
        cmdstruct->type = PIPE;
        if (cmdstruct->pipe == NULL)
        {
            perror("erreur remplissage pipe");
        }
    }
    else
    {
        cmdstruct->cmdSimple = remplissage_cmdSimple(args);
        if(cmdstruct->cmdSimple->type){
            cmdstruct->type = CMD_INTERNE;
        }else{
            cmdstruct->type = CMD_EXTERNE;
        }
        if (!cmdstruct->cmdSimple)
        {
            perror("Erreur cmdSimple");
        }
    }
}

int exec_redirection(cmd_redirection *cmd)
{
    // tester les cmd->separateur et appelé les fonctions approriés et retourné la valeur de retour de ses fonctions
    return 0;
}

int exec_pipe(cmd_pipe *cmd)
{
    return cmdpipe(cmd);
}

int fsh(char *input, char *chemin, int *dernier_exit, commandeStruct *cmdstruct)
{
    int ret = 0;

    /*gestion de la commande Simple pour l'instant cad CMD_INTERNE && CMD_EXTERNE*/
    if (cmdstruct == NULL)
    {
        perror("Structure commande");
        return -1;
    }

    // exit
    // TODO testé direct si cmdstruct->type = CMD_INTERNE sinon problème psq si cmd==NULL erreur 
    if (cmdstruct->type == CMD_INTERNE)
    {
        char *cmd = cmdstruct->cmdSimple->args[0];
        char premierchar = cmdstruct->cmdSimple->args[0][0];
        char *arg = cmdstruct->cmdSimple->args[1];
        if (strcmp(cmd, "exit") == 0)
        {
            *dernier_exit = commande_exit(arg); // TODO : donné a ces fonctions directe la strcuture??
            if (*dernier_exit == -5)
            {
                ret = -5;
                *dernier_exit = 0;
                if (input)
                    free(input);
                if (chemin)
                    free(chemin);
                /*free(cmdstruct->cmdSimple->args);
                free(cmdstruct->cmdSimple);
                free(cmdstruct);*/
                freeCmdStruct(cmdstruct);
                return ret;
            }
            if (input)
                free(input);
            if (chemin)
                free(chemin);
            /*free(cmdstruct->cmdSimple->args);
            free(cmdstruct->cmdSimple);
            free(cmdstruct);*/
            freeCmdStruct(cmdstruct);
            exit(*dernier_exit);
        }

        // gestion de cd
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

        // gestion de pwd

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
            ret = boucle_for(cmdstruct->cmdFor);
            if (ret != 0)
            {
                perror("boucle_for");
                return ret;
            };
        }
        else if (premierchar == '\0' || premierchar == '\n' || premierchar == ' ' || premierchar == '\t')
        {
            ret = *dernier_exit;
        }
    }
    else if (cmdstruct->type == REDIRECTION)
    { // Pas sur de celle la mais ca me parait logique
        // Alternatif = cmdstruct->cmdRed !=NULL ou cmdstrcut->cmdRed->type == REDIRECTION
        ret = exec_redirection(cmdstruct->cmdRed);
    }
    else if (cmdstruct->type == PIPE)
    {
        ret = exec_pipe(cmdstruct->pipe);
    }
    else if(cmdstruct->type == CMD_EXTERNE)
    {
        ret = cmd_extern(cmdstruct->cmdSimple);
        if (ret < 0)
        {
            return ret;
        }
    }
    return ret;
}