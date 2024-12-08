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

        if(cmdstruct->cmdSimple){
            if(cmdstruct->cmdSimple->args){
                for(int i = 0 ; cmdstruct->cmdSimple->args && cmdstruct->cmdSimple->args[i] ; i++ ){
                free(cmdstruct->cmdSimple->args[i]);
            }
            free(cmdstruct->cmdSimple->args);
            }
            
            free(cmdstruct->cmdSimple);
        }
        free(cmdstruct);

        dernier_exit = ret;
    }
}
