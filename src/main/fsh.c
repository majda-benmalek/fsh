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
#include "../../utils/gestionStruct.h"

int dernier_exit = 0; // pour initialiser la derniére valeur de retour

int main(void)
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
    commandeStruct *cmdstruct = malloc(sizeof(commandeStruct));
    if (cmdstruct == NULL)
    {
        perror("erreur malloc cmdStruct");
        free(input);
        free(chemin);
        exit(1);
    }
    int ret = 0;
    while (1)
    {
        cmdstruct = remplissage_cmdStruct(CMD_STRUCT, NULL, NULL, NULL, NULL,NULL, 0, cmdstruct); //tout initialisé a NULL
        int r = prompt(chemin, input, &ret);
        if (r == 1) // Ctrl-D pressed
        {
            char *v_exit = malloc(2);
            sprintf(v_exit, "%d", dernier_exit);
            dernier_exit = commande_exit(v_exit);
            if (input != NULL)
                free(input);
            if (chemin != NULL)
                free(chemin);
            if (cmdstruct != NULL)
                freeCmdStruct(cmdstruct);
            if (v_exit != NULL)
                free(v_exit);
            exit(dernier_exit);
        }
        gestion_cmd(input, cmdstruct);
        // if (cmdstruct->cmdFor == NULL){
        //     printf("aie aie aie ...\n");
        //     printf("cmdfor -> var = %s\n",cmdstruct->cmdFor->variable);
        //     printf("rep = %s\n",cmdstruct->cmdFor->rep);
        // }
        // else{
        //     printf("cmdFor!= null dans main \n");
        // }
        ret = fsh(chemin, &dernier_exit, cmdstruct);
        dernier_exit = ret;
    }
}