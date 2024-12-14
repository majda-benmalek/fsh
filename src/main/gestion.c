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
#include "../../utils/redirection.h"
#include "../../utils/pipe.h"
#define ARG_MAX 512

int rechercheDansArgs(char *tofind, char **args)
{
    for (int i = 0; i < tailleArgs(args) - 1; i++)
    {
        if (strcmp(args[i], tofind) == 0)
        {
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
    // print args
    // for (int i = 0; i <= nb_args; i++)
    // {
    //     printf("args[%d] : %s\n", i, args[i]);
    // }

    if (args[0] == NULL)
    {
        return;
    }
    else if (strcmp(args[0],"for") == 0){
        // cmdstruct->cmdFor = malloc(sizeof(cmdFor));
        cmdstruct->cmdFor = make_for(args);
        cmdstruct->type = FOR;
        if (cmdstruct->cmdFor == NULL){
            freeCmdStruct(cmdstruct);
            perror("Erreur remplissage de for");
        }
    }
    else
        // REDIRECTION
        /*if (strstr(input, ">") != NULL || strstr(input, "<") != NULL || strstr(input, ">>") != NULL || strstr(input, ">|") != NULL || strstr(input, "2>") != NULL || strstr(input, "2>>") != NULL || strstr(input, "2>|") != NULL)
        {
            cmdstruct->cmdRed = remplissageCmdRedirection(args);
            cmdstruct->type = REDIRECTION;
            if (cmdstruct->cmdRed == NULL)
            {
                perror("Erreur remplissage redirection");
            }
        }*/
       if(rechercheDansArgs(">" , args) || rechercheDansArgs(">>" , args) || rechercheDansArgs("<" , args) || rechercheDansArgs(">|" , args) || rechercheDansArgs("2>" , args) || rechercheDansArgs("2>>" , args) || rechercheDansArgs("2>|" , args)){
            cmdstruct->cmdRed = remplissageCmdRedirection(args);
            cmdstruct->type = REDIRECTION;
            if (cmdstruct->cmdRed == NULL)
            {
                perror("Erreur remplissage redirection");
            }
       }
        else if (rechercheDansArgs("|", args))
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
            if (cmdstruct->cmdSimple->type==CMD_INTERNE)
            {
                cmdstruct->type = CMD_INTERNE;
            }
            else if(cmdstruct->cmdSimple->type==CMD_EXTERNE)
            {
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
    return redirection(cmd);
}

int exec_pipe(cmd_pipe *cmd)
{
    return cmdpipe(cmd);
}

int fsh(char *chemin, int *dernier_exit, commandeStruct *cmdstruct)
{
    // printf("dans la méthode fsh\n");
    int ret = *dernier_exit;

    /*gestion de la commande Simple pour l'instant cad CMD_INTERNE && CMD_EXTERNE*/
    if (cmdstruct == NULL)
    {
        perror("Structure commande");
        return -1;
    }
    
    if (cmdstruct->type == FOR){
            printf("remontada du for\n");
            ret = boucle_for(cmdstruct->cmdFor);
            if (ret != 0)
            {
                perror("boucle_for");
                return ret;
            };
    }
    // exit
    // TODO testé direct si cmdstruct->type = CMD_INTERNE sinon problème psq si cmd==NULL erreur
    if (cmdstruct->type == CMD_INTERNE)
    {
        // printf("dans cmd interne\n");
        char *cmd = cmdstruct->cmdSimple->args[0];
        // // char premierchar = cmdstruct->cmdSimple->args[0][0]; //gérer dans gestion
        char *arg = cmdstruct->cmdSimple->args[1];
        if (strcmp(cmd, "exit") == 0)
        {
            if (cmdstruct->cmdSimple->args[1] != NULL && cmdstruct->cmdSimple->args[2] != NULL) // le && c'est pour evité d'essaye de lire une partie de la mémoire qui existe pas
            {
                write(2, "exit: too many arguments\n", strlen("exit: too many arguments\n"));
                ret = 1;
                return ret;
            }
            *dernier_exit = commande_exit(arg); // TODO : donné a ces fonctions directe la strcuture??
            //! je me rappel c'etait pourquoi cette partie pardon
            // if (*dernier_exit == -4)
            // {
            //     ret = -4;
            //     *dernier_exit = 0;
            // if (input)
            //     free(input);
            // if (chemin)
            //     free(chemin);
            // freeCmdStruct(cmdstruct);
            //     return ret;
            // }
            // if (input)
            //     free(input);
            if (chemin)
                free(chemin);
            if (cmdstruct)
                freeCmdStruct(cmdstruct);
            exit(*dernier_exit);
        }

        // gestion de cd
        else if (strcmp(cmd, "cd") == 0)
        {
            if (cmdstruct->cmdSimple->args[1] != NULL && cmdstruct->cmdSimple->args[2] != NULL)
            {
                write(2, "cd: too many arguments\n", strlen("cd: too many arguments\n"));
                ret = 1;
                return ret;
            }
            ret = cd_commande(arg);
            if (getcwd(chemin, PATH_MAX) == NULL)
            {
                perror("getcwd");
                ret = 1;
                return ret;
            }
            return ret;
        }
        // gestion de pwd
        else if (strcmp(cmd, "pwd") == 0)
        {
            if (cmdstruct->cmdSimple->args[1] != NULL)
            {
                write(2, "pwd: too many arguments\n", strlen("pwd: too many arguments\n"));
                ret = 1;
                return ret;
            }
            ret = pwd();
        }
        else if (strcmp(cmd, "ftype") == 0)
        {
            if (cmdstruct->cmdSimple->args[1] != NULL && cmdstruct->cmdSimple->args[2] != NULL)
            {
                write(2, "ftype: too many arguments\n", strlen("ftype: too many arguments\n"));
                ret = 1;
                return ret;
            }
            ret = ftype(arg);
            if (ret > 0)
            {
                perror("ftype");
                return ret;
            }
        }
    }
    else if (cmdstruct->type == REDIRECTION)
    {
        ret = exec_redirection(cmdstruct->cmdRed);
    }
    else if (cmdstruct->type == PIPE)
    {
        ret = exec_pipe(cmdstruct->pipe);
    }
    else if (cmdstruct->type == CMD_EXTERNE)
    {
        ret = cmd_extern(cmdstruct->cmdSimple);
        if (ret < 0)
        {
            return ret;
        }
    }
    return ret;
}