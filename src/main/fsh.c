#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "../../utils/exit.h"
#include "../../utils/prompt.h"
#include <stdbool.h>
#include "../../utils/gestion.h"
#include <linux/limits.h>
#include "../../utils/commande.h"
#include "../../utils/gestionStruct.h"
#include "../../utils/freeStruct.h"
#include "../../utils/commandeStructuree.h"
#include "../../utils/signaux.h"


int dernier_exit = 0;
 // pour initialiser la derniére valeur de retour

int main(void)
{

    // signaux 
    sigaux_main();
   

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
        free(input);
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
       
        
        commandeStruct *cmdstruct = remplissage_cmdStruct(CMD_STRUCT, NULL, NULL, NULL, NULL,NULL, NULL, 0, NULL);
        if (cmdstruct == NULL)
        {
            perror("erreur malloc cmdStruct");
            free(input);
            free(chemin);
            // if (cmdstruct != NULL)
            //     freeCmdStruct(cmdstruct); // ? si c null y'a rien a free nan ?
            exit(1);
        }
        // cmdstruct = remplissage_cmdStruct(CMD_STRUCT, NULL, NULL, NULL, NULL,NULL, NULL, 0, cmdstruct); // tout initialisé a NULL

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

        // Mettre input sous forme de tableau
        char *args[ARG_MAX] = {NULL};
        int nb_args = 0;
        char *token = strtok(input, " \t"); // pour gerer le cas ou l'utilisateur separe les arguments avec tab
        while (token && nb_args < ARG_MAX - 1)
        {
            args[nb_args++] = token;
            token = strtok(NULL, " \t");
        }
        args[nb_args] = NULL;
        
        gestion_cmd(args, cmdstruct);
        ret = fsh(chemin, &dernier_exit, cmdstruct);
        dernier_exit = ret;
        if (cmdstruct != NULL){
            freeCmdStruct(cmdstruct);}
    }
}