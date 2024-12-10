#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include "../../utils/exit.h"
#include "../../utils/prompt.h"
#include <stdbool.h>
#include "../../utils/gestion.h"
#include <linux/limits.h>
#include "../../utils/commande.h"
#include "../../utils/gestionStruct.h"

int dernier_exit = 0; // pour initialiser la derniére valeur de retour

int main(void){
    char *input = malloc(MAX_INPUT);
    if (input == NULL)
    {
        perror("malloc");
        return 1;
    }
    char *chemin = malloc(PATH_MAX);
    if (chemin == NULL)
    {
        perror("malloc");
        return 1;
    }

    if (getcwd(chemin, PATH_MAX) == NULL)
    {
        perror("getcwd");
        free(input);
        free(chemin);
        return 1;
    }
    rl_outstream = stderr;

    int ret = 0;
    while (1)
    {
        int r = prompt(chemin, input, &ret);
        if (r == 1) // Ctrl-D pressed
        {
            dernier_exit = commande_exit(NULL);
            if (input != NULL) free(input);
            if (chemin != NULL) free(chemin);
            exit(dernier_exit);
        }
        commandeStruct *cmdstruct = malloc(sizeof(commandeStruct));
        if (cmdstruct == NULL){
            perror("erreur malloc cmdStruct");
            free(input);
            free(chemin);
            exit(1);
        }
        gestion_cmd(input,cmdstruct);
        ret = fsh(input, chemin, &dernier_exit , cmdstruct);
        freeCmdStruct(cmdstruct);
        dernier_exit = ret;
    }
}






/*int main(void)
{
    char *input = malloc(MAX_INPUT);
    if (input == NULL)
    {
        perror("malloc");
        return 1;
    }

    char *chemin = malloc(PATH_MAX);
    if (chemin == NULL)
    {
        perror("malloc");
        return 1;
    }

    if (getcwd(chemin, PATH_MAX) == NULL)
    {
        perror("getcwd");
        free(input);
        free(chemin);
        return 1;
    }
    rl_outstream = stderr;

    int ret = 0;

    char *arg = malloc(ARG_MAX);
    char *cmd = malloc(ARG_MAX);
    if (arg == NULL || cmd == NULL)
    {
        perror("malloc");
        return 1;
    }

    while (1)
    {
        int r = prompt(chemin, input, &ret);
        if (r == 1) // Ctrl-D pressed
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
        gestion_cmd(input, arg, cmd);
        ret = fsh(cmd, arg, input, chemin, &dernier_exit);
        dernier_exit = ret;
    }
}*/
