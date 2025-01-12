// GESTION STRUCT
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <linux/limits.h>
#include "../../utils/commande.h"
#include "../../utils/gestion.h"
#include "../../utils/for.h"
#include "../../utils/freeStruct.h"
#include "../../utils/exit.h"
#include "../../utils/commandeStructuree.h"
#include <assert.h>

cmd_redirection *remplissageCmdRedirection(char **args);

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
            for (int i = 0; i < h; i++)
            {
                free(commande[i]);
            }
            free(commande);
            return 1;
        }
    }
    commande[i - j] = NULL; // pour le dernier élement
    return 0;
}

commandeStruct *remplissage_cmdStruct(Type type, cmd_simple *cmdSimple, cmd_pipe *pipestruct, cmdIf *cmdIfStruct, cmdFor *cmdForStruct, commandeStruct **cmdsStrcu, int nbcommandes, commandeStruct *cmd)
{

    if (cmd == NULL)
    {
        cmd = malloc(sizeof(commandeStruct));
        if (cmd == NULL)
        {
            perror("malloc remplissage_cmdStruct");
            return NULL;
        }
    }
    cmd->type = type;
    cmd->cmdSimple = cmdSimple;
    cmd->pipe = pipestruct;
    cmd->cmdIf = cmdIfStruct;
    cmd->cmdFor = cmdForStruct;
    cmd->nbCommandes = nbcommandes;
    cmd->cmdsStruc = cmdsStrcu;

    if (cmd != NULL)
    {
        assert(cmd->type == type);
        assert(cmd->cmdSimple == cmdSimple);
        assert(cmd->pipe == pipestruct);
        assert(cmd->cmdIf == cmdIfStruct);
        assert(cmd->cmdFor == cmdForStruct);
        assert(cmd->nbCommandes == nbcommandes);
        assert(cmd->cmdsStruc == cmdsStrcu);
    }
    else
    {
        perror("null");
    }
    return cmd;
}

cmd_simple *remplissage_cmdSimple(char **args)
{
    cmd_simple *cmd = malloc(sizeof(cmd_simple));
    if (cmd == NULL)
    {
        perror("malloc CommandSimple");
        return NULL;
    }
    cmd->red = NULL;

    int nbargs = 0;
    while (args[nbargs])
    {
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

    if (strcmp(cmd->args[0], "cd") == 0 || strcmp(cmd->args[0], "pwd") == 0 || strcmp(cmd->args[0], "ftype") == 0 || strcmp(cmd->args[0], "exit") == 0)
    {
        cmd->type = CMD_INTERNE;
    }
    else
    {
        cmd->type = CMD_EXTERNE;
    }
    if (rechercheDansArgs(">", args) || rechercheDansArgs(">>", args) || rechercheDansArgs("<", args) || rechercheDansArgs(">|", args) || rechercheDansArgs("2>", args) || rechercheDansArgs("2>>", args) || rechercheDansArgs("2>|", args))
    {
        cmd->red = remplissageCmdRedirection(args);
        if (cmd->red == NULL)
        {
            free_redirection(cmd->red);
            return NULL;
        }
        cmd->type = REDIRECTION;
        return cmd;
    }

    return cmd;
}

cmd_redirection *remplissageCmdRedirection(char **args)
{
    cmd_redirection *cmd = malloc(sizeof(cmd_redirection));
    size_t taille = tailleArgs(args);
    if (cmd == NULL)
    {
        perror("malloc");
        return NULL;
    }

    char **commande = malloc(10 * sizeof(char *));
    if (commande == NULL)
    {
        perror("malloc commande");
        free(cmd);
        return NULL;
    }
    memset(commande, 0, 10 * sizeof(char *));
    if (arg_cmdsimple(args, commande, taille - 3, 0) == 1)
    {
        for (int i = 0; commande[i] != NULL; i++)
        {
            free(commande[i]);
        }
        free(commande);
        perror("strdup cmd_redirection");
        free(cmd);
        return NULL;
    }
    size_t pos_sep = tailleArgs(args) - 3;
    cmd->cmd = remplissage_cmdSimple(commande);
    if (cmd->cmd == NULL)
    {
        perror("remplissage cmd simple");
        for (int i = 0; commande[i] != NULL; i++)
        {
            free(commande[i]);
        }
        free(commande);
        free(cmd);
        return NULL;
    }
    if (strcmp(args[pos_sep], ">") == 0)
    {
        cmd->fichier = strdup(args[taille - 2]);
        cmd->separateur = ">";
    }
    else if (strcmp(args[pos_sep], ">>") == 0)
    {
        cmd->fichier = strdup(args[taille - 2]);
        cmd->separateur = ">>";
    }
    else if (strcmp(args[pos_sep], "2>") == 0)
    {
        cmd->fichier = strdup(args[taille - 2]);
        cmd->separateur = "2>";
    }
    else if (strcmp(args[pos_sep], ">|") == 0)
    {
        cmd->fichier = strdup(args[taille - 2]);
        cmd->separateur = ">|";
    }
    else if (strcmp(args[pos_sep], "2>|") == 0)
    {
        cmd->fichier = strdup(args[taille - 2]);
        cmd->separateur = "2>|";
    }
    else if (strcmp(args[pos_sep], "2>>") == 0)
    {
        cmd->fichier = strdup(args[taille - 2]);
        cmd->separateur = "2>>";
    }
    else if (strcmp(args[pos_sep], "<") == 0)
    {
        cmd->fichier = strdup(args[taille - 2]);
        cmd->separateur = "<";
    }
    else
    {
        perror("Erreur de syntaxe");
        for (int i = 0; commande[i] != NULL; i++)
        {
            free(commande[i]);
        }
        free(commande);
        free(cmd);
        return NULL;
    }
    for (int i = 0; commande[i] != NULL; i++)
    {
        free(commande[i]);
    }
    free(commande);
    return cmd;
}

cmd_pipe *remplissageCmdPipe(char **args)
{
    cmd_pipe *cmd = malloc(sizeof(cmd_pipe));
    cmd->commandes = malloc(ARG_MAX * sizeof(cmd_simple));
    int nb = 0;
    int j = 0;

    // TODO tableau dynamique
    char *commande[ARG_MAX];
    memset(commande, 0, sizeof(commande));
    for (size_t i = 0; i < tailleArgs(args); i++)
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
            if (args[i + 1] == NULL)
            {
                perror("Syntaxe error");
                free_pipe(cmd);
                return NULL;
            }
            else
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
        for (int i = 0; commande[i] != NULL; i++)
        {
            free(commande[i]);
        }
    }
    cmd->type = PIPE;
    cmd->nbCommandes = nb;
    cmd_simple **temp = realloc(cmd->commandes, cmd->nbCommandes * sizeof(cmd_simple *));
    if (temp == NULL)
    {
        perror("realloc cmd->commandes");
        free_pipe(cmd);
        return NULL;
    }
    cmd->commandes = temp;


    return cmd;
}

void remplissageCmdStructurees(char **args, commandeStruct *cmdStruct)
{
    cmdStruct->cmdsStruc = malloc(sizeof(commandeStruct *) * ARG_MAX);
    int nbCommandes = decoupe_args(args, cmdStruct->cmdsStruc, ARG_MAX);
    if (nbCommandes < 0 && cmdStruct->cmdsStruc == NULL)
    {
        freeCmdStruct(cmdStruct);
        perror("Erreur découpages commandes structurées");
        return;
    }
    cmdStruct->nbCommandes = nbCommandes;
    cmdStruct->type = CMD_STRUCT;
}

cmdFor *make_for(char **args)
{
    cmdFor *cmdFor = malloc(sizeof(*cmdFor));
    if (cmdFor == NULL)
    {
        perror("problème d'allocation de mémoire pour for");
        return NULL;
    }
    if (tailleArgs(args) < 9)
    {
        perror("Erreur de synatxe");
        printf("la taille de l'argument = %ld\n", tailleArgs(args));
        for (size_t i = 0; i < tailleArgs(args); i++)
        {
            printf("%s\n", args[i]);
        }
        return NULL;
    }
    cmdFor->rep = NULL;
    cmdFor->op = NULL;
    cmdFor->variable = NULL;
    cmdFor->cmd = remplissage_cmdStruct(CMD_STRUCT, NULL, NULL, NULL, NULL, NULL, 0, NULL);
    // ? -------- Type ---------
    cmdFor->type = FOR;

    // * ------------------ variable ---------------
    if (strlen(args[1]) != 1)
    {
        perror("Erreur de syntaxe, la variabme doit contenir un seul caractère");
        return NULL;
    }

    size_t taille = tailleArgs(args);
    cmdFor->variable = strdup(args[1]);
    if (cmdFor->variable == NULL)
    {
        perror("erreur de duuuup");
        free_for(cmdFor);
        return NULL;
    }

    cmdFor->rep = strdup(args[3]);
    cmdFor->op = malloc(12 * sizeof(char *));
    if (cmdFor->op == NULL)
    {
        perror("aie aie aie");
        free_for(cmdFor);
        return NULL;
    }
    memset(cmdFor->op, 0, 12 * sizeof(char *));
    // ? ----------------- option-----------
    size_t i = 4;
    size_t j = 0;
    while (strcmp(args[i], "{") != 0)
    {
        if (strcmp(args[i], "-A") == 0 || strcmp(args[i], "-r") == 0)
        {
            cmdFor->op[j] = strdup(args[i]); // TODO utiliser strdup
            if (cmdFor->op[j] == NULL)
            {
                perror("strdup for");
                free_for(cmdFor);
                return NULL;
            }
            i++;
            j++;
        }
        else if (strcmp(args[i], "-e") == 0 || strcmp(args[i], "-t") == 0 || strcmp(args[i], "-p") == 0)
        {
            if (args[i + 1][0] != '-' && args[i + 1][0] != '{')
            {
                cmdFor->op[j] = strdup(args[i]); // TODO a changer
                if (cmdFor->op[j] == NULL)
                {
                    perror("strdup for");
                    free_for(cmdFor);
                    return NULL;
                }
                cmdFor->op[j + 1] = strdup(args[i + 1]);
                if (cmdFor->op[j + 1] == NULL)
                {
                    perror("strdup for");
                    free_for(cmdFor);
                    return NULL;
                }
                i = i + 2;
                j = j + 2;
                // flag = true;
            }
            else
            {
                perror("il manque un argument");
                free_for(cmdFor);
                return NULL;
            }
        }
    }
    cmdFor->op[j] = NULL;
    if (strcmp(args[i], "{") == 0)
    {
        i++; // pour sauter l'{
    }
    char *tab[ARG_MAX];
    size_t k = 0;
    while (args[i] != NULL && i < taille - 2) // sauter { et le null
    {                                        
        tab[k] = args[i];
        k = k + 1;
        i = i + 1;
    }

    tab[k] = NULL;
    remplissageCmdStructurees(tab, cmdFor->cmd);
    return cmdFor;
}

cmdIf *remplissageCmdIf(char **args)
{
    cmdIf *cmd = malloc(sizeof(cmdIf));
    cmd->type = IF;
    cmd->commandeIf = remplissage_cmdStruct(CMD_STRUCT, NULL, NULL, NULL, NULL, NULL, 0, NULL);
    cmd->commandeElse = remplissage_cmdStruct(CMD_STRUCT, NULL, NULL, NULL, NULL, NULL, 0, NULL);
    cmd->test = NULL;

    if (strcmp(args[1], "{") == 0)
    {
        perror("Syntaxe error");
        free_if(cmd);
        return NULL;
    }

    // ! TEST
    size_t taille = tailleArgs(args);
    int i = 0;
    while (i < taille && strcmp(args[i], "{") != 0)
    {
        i++;
    }
    // a la fin i= la pos de {

    char **commande = malloc(ARG_MAX * sizeof(char *));
    for (int k = 0; k < ARG_MAX; k++)
    {
        commande[k] = NULL;
    }
    if (arg_cmdsimple(args, commande, i, 1) == 1) // copie de 1 à i (exclu)
    {
        perror("arg_cmdsimple");
        for (int i = 0; commande[i] != NULL; i++)
        {
            free(commande[i]);
        }
        free(commande);
        free_if(cmd);
        return NULL;
    }
    // TODO : appelé gestion psq ca peut etre une redirection aussi
    cmd->test = remplissage_cmdStruct(CMD_STRUCT, NULL, NULL, NULL, NULL, NULL, 0, NULL);
    gestion_cmd(commande, cmd->test);
    if (cmd->test == NULL)
    {
        perror("remplissageCmdPipe");
        free(commande);
        free_if(cmd);
        return NULL;
    }
    for (int i = 0; commande[i] != NULL; i++)
    {
        free(commande[i]);
    }
    // ! commande dans le if
    memset(commande, 0, ARG_MAX * sizeof(char *));
    int j = i + 1; // début de la commande a éxécuté

    int fin = j;
    int imbrication = 0;
    bool estDansBloc = false;
    // trouver la fin du bloc
    // ! FONCTION by SABRINA
    while (args[fin] != NULL)
    {
        if (strcmp(args[fin], "{") == 0)
        {
            imbrication++;
            estDansBloc = true;
        }
        else if (strcmp(args[fin], "}") == 0)
        {
            imbrication--;
            if (imbrication == 0)
                estDansBloc = false;
        }
        else if ((!estDansBloc && (strcmp(args[fin], "else") == 0 || (strcmp(args[fin - 1], "}") == 0 && strcmp(args[fin], ";") == 0))) || args[fin] == NULL)
        {
            break;
        }
        fin++;
    }
    for (int i = 0; commande[i] != NULL; i++)
    {
        free(commande[i]);
    }
    memset(commande, 0, ARG_MAX * sizeof(char *));

    if (arg_cmdsimple(args, commande, fin - 1, j) == 1) //
    {
        perror("arg_cmdsimple");
        for (int i = 0; commande[i] != NULL; i++)
        {
            free(commande[i]);
        }
        free(commande);
        free_if(cmd);
        return NULL;
    }
    remplissageCmdStructurees(commande, cmd->commandeIf);
    for (int i = 0; commande[i] != NULL; i++)
    {
        free(commande[i]);
    }
    memset(commande, 0, ARG_MAX * sizeof(char *));

    // ! commande dans le else
    j = fin; // le else ou pas
    fin = j;
    imbrication = 0;
    estDansBloc = false;

    if (args[j] != NULL)
    {
        if (strcmp(args[j], "else") == 0)
        {
            if (strcmp(args[j + 1], "{") != 0)
            {
                perror("erreur de syntaxe");
                if (commande != NULL)
                {
                    for (int i = 0; commande[i] != NULL; i++)
                    {
                        free(commande[i]);
                    }
                    free(commande);
                }
                if (cmd != NULL)
                    free_if(cmd);
                return NULL;
            }
            while (args[fin] != NULL)
            {
                if (strcmp(args[fin], "{") == 0)
                {
                    imbrication++;
                    estDansBloc = true;
                }
                else if (strcmp(args[fin], "}") == 0)
                {
                    imbrication--;
                    if (imbrication == 0)
                        estDansBloc = false;
                }
                else if ((!estDansBloc && (strcmp(args[fin], "}") == 0)) || args[fin] == NULL)
                {
                    break;
                }
                fin++;
            }
            if (arg_cmdsimple(args, commande, fin - 1, j + 2) == 1)
            {
                perror("arg_cmdsimple");
                if (commande != NULL)
                {
                    for (int i = 0; commande[i] != NULL; i++)
                    {
                        free(commande[i]);
                    }
                    free(commande);
                }
                if (cmd != NULL)
                    free_if(cmd);
                return NULL;
            }
            remplissageCmdStructurees(commande, cmd->commandeElse);
        }
    }

    if (commande != NULL)
    {
        for (int h = 0; commande[h] != NULL; h++)
        {
            free(commande[h]);
        }
        free(commande);
    }
    return cmd;
}
