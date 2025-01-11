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
#include "../../utils/commandeStructuree.h"
#include "../../utils/cmdStructuree.h"
#include "../../utils/redirection.h"
#include "../../utils/pipe.h"
#include "../../utils/freeStruct.h"
#include "../../utils/if.h"
#include "../../utils/signaux.h"

int rechercheDansArgs(char *tofind, char **args)
{
    for (int i = 0; i < tailleArgs(args) - 1; i++)
    {

        if (strcmp(args[i], tofind) == 0)
        {

            return 1; // vrai
        }
    }
    return 0; // faux
}

void gestion_cmd(char **args, commandeStruct *cmdstruct)
{
    if (!cmdstruct)
    {
        perror("Erreur Structure");
        return;
    }
    else if (args[0] == NULL || args[0][0] == '\0' || args[0][0] == '\n')
    {
        return;
    }
    if (strcmp(args[0], "if") == 0)
    {
        if (trouver_fin_bloc(args, 0) < tailleArgs(args))
        {
            // ! if [ 1 -eq 1 ] { echo 3 ; if [ 1 -eq 1 ] { echo ok ; echo 4 } } ; ls
            if (args[trouver_fin_bloc(args, 0)] == NULL)
            {
                if (strcmp(args[trouver_fin_bloc(args, 0) - 1], ";") != 0)
                {
                    // perror("je suis un if ");
                    cmdstruct->cmdIf = remplissageCmdIf(args);
                    cmdstruct->type = IF;
                    if (cmdstruct->cmdIf == NULL)
                    {
                        free_if(cmdstruct->cmdIf);
                        perror("erreur remplissage if");
                    }
                    return;
                }
            }
        }
    }
    else if (strcmp(args[0], "for") == 0 || cmdstruct->type == FOR)
    {
        if (trouver_fin_bloc(args, 0) < tailleArgs(args))
        {
            if (args[trouver_fin_bloc(args, 0)] == NULL)
            {
                if (strcmp(args[trouver_fin_bloc(args, 0) - 1], ";") != 0)
                {
                    cmdstruct->cmdFor = make_for(args);
                    cmdstruct->type = FOR;
                    if (cmdstruct->cmdFor == NULL)
                    {
                        perror("Erreur remplissage de for");
                    }
                }
            }
        }
        return;
    }
    else if (rechercheDansArgs(";", args))
    {
        int debutBloc = -1, finBloc = -1;
        bool pvoutbloc = false;

        // arriver a la fin du bloc
        for (int i = 0; args[i] != NULL; i++)
        {
            if (strcmp(args[i], "{") == 0)
            {
                debutBloc = i;
            }
            else if (strcmp(args[i], "}") == 0 && debutBloc != -1)
            {
                finBloc = i;
            }
            if (strcmp(args[i], ";") == 0 && ((debutBloc == -1 || i < debutBloc || i > finBloc)))
            {
                pvoutbloc = true;
                break;
            }
        }

        // une fois la fin du bloc detectecté tester si ya un ; apres
        if (pvoutbloc)
        {

            remplissageCmdStructurees(args, cmdstruct);
            if (cmdstruct->cmdsStruc == NULL)
            {
                perror("Erreur d'allocation de mémoire ou découpage des arguments échoué");
                freeCmdStruct(cmdstruct);
                return;
            }

            if (cmdstruct->nbCommandes < 0)
            {
                perror("Erreur lors du découpage des commandes");
                freeCmdStruct(cmdstruct); //
                return;
            }
            cmdstruct->type = CMD_STRUCT;
        }
        return;
    }
    else if (rechercheDansArgs("|", args))
    {
        cmdstruct->pipe = remplissageCmdPipe(args);
        cmdstruct->type = PIPE;
        if (cmdstruct->pipe == NULL)
        {
            perror("erreur remplissage pipe");
        }
        return;
    }
    else
    {
        cmdstruct->cmdSimple = remplissage_cmdSimple(args);
        if (!cmdstruct->cmdSimple)
        {
            perror("Erreur cmdSimple");
        }
        if (cmdstruct->cmdSimple->type == CMD_INTERNE)
        {
            cmdstruct->type = CMD_INTERNE;
            return;
        }
        else if (cmdstruct->cmdSimple->type == CMD_EXTERNE)
        {
            cmdstruct->type = CMD_EXTERNE;
            return;
        }
        else if (cmdstruct->cmdSimple->type == REDIRECTION)
        {
            cmdstruct->type = REDIRECTION;
            return;
        }
    }
}

int exec_pipe(cmd_pipe *cmd)
{
    return cmdpipe(cmd);
}

int fsh(char *chemin, int *dernier_exit, commandeStruct *cmdstruct)
{
    if (sigint_received)
    {
        return -255;
    }
    int ret = *dernier_exit;

    if (cmdstruct == NULL)
    {
        perror("Structure commande");
        return -1;
    }

    if (cmdstruct->type == FOR)
    {
        if (cmdstruct->cmdFor != NULL)
        {
            ret = boucle_for(cmdstruct->cmdFor);
            if (ret == 1 || ret == 0)
            {
                return ret;
            }
            else
            {
                return max;
            }
        }
    }
    else if (cmdstruct->type == CMD_INTERNE)
    {
        char *cmd = cmdstruct->cmdSimple->args[0];
        char *arg = cmdstruct->cmdSimple->args[1];
        for (int i = 0; cmdstruct->cmdSimple->args[i] != NULL; i++)
        {
            printf("fsh simple args[%d] = [%s]\n", i, cmdstruct->cmdSimple->args[i]);
        }
        if (strcmp(cmd, "exit") == 0)
        {
            // ! c'est ça qui fais invalid read (test sur un truc qui est NULL)
            if (tailleArgs(cmdstruct->cmdSimple->args) > 3)
            {
                write(2, "exit: too many arguments\n", strlen("exit: too many arguments\n"));
                ret = 1;
                return ret;
            }
            *dernier_exit = commande_exit(arg);
            if (chemin)
                free(chemin);
            if (cmdstruct)
                freeCmdStruct(cmdstruct);
            exit(*dernier_exit);
        }

        // gestion de cd
        else if (strcmp(cmd, "cd") == 0)
        {
            if (tailleArgs(cmdstruct->cmdSimple->args) > 3)
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
            if (tailleArgs(cmdstruct->cmdSimple->args) > 2)
            {
                write(2, "pwd: too many arguments\n", strlen("pwd: too many arguments\n"));
                ret = 1;
                return ret;
            }
            ret = pwd();
            return ret;
        }
        else if (strcmp(cmd, "ftype") == 0)
        {
            if (tailleArgs(cmdstruct->cmdSimple->args) > 3)
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
    else if (cmdstruct->type == PIPE)
    {
        ret = exec_pipe(cmdstruct->pipe);
    }
    else if (cmdstruct->type == CMD_EXTERNE)
    {
        for (int i = 0; cmdstruct->cmdSimple->args[i] != NULL; i++)
        {
            printf("fsh externe args[%d] = [%s]\n", i, cmdstruct->cmdSimple->args[i]);
        }
        ret = cmd_extern(cmdstruct->cmdSimple);
        return ret;
    }
    else if (cmdstruct->type == REDIRECTION && cmdstruct->cmdSimple->red != NULL)
    {
        for (int i = 0; cmdstruct->cmdSimple->red->cmd->args[i] != NULL; i++)
        {
            printf("fsh redirection args[%d] = [%s]\n", i, cmdstruct->cmdSimple->red->cmd->args[i]);
        }
        return redirection(cmdstruct->cmdSimple->red);
    }
    else if (cmdstruct->type == CMD_STRUCT && cmdstruct->cmdsStruc != NULL)
    {
        ret = execCmdStruct(cmdstruct->cmdsStruc, cmdstruct->nbCommandes, chemin);
        return ret;
    }
    else if (cmdstruct->type == IF && cmdstruct->cmdIf != NULL)
    {
        ret = exec_if(cmdstruct->cmdIf, chemin);
    }
    return ret;
}