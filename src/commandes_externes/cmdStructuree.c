#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <wait.h>
#include "../../utils/commande.h"
#include "../../utils/extern.h"
#include "../../utils/gestion.h"
#include "../../utils/gestionStruct.h"
#include "../../utils/exit.h"
#include "../../utils/freeStruct.h"


int execCmdStruct(commandeStruct ** cmds , int nbCommandes){
    if (cmds == NULL || nbCommandes <= 0) {
        fprintf(stderr, "Erreur : tableau de commandes invalide ou vide\n");
        return -1;
    }


    pid_t pid_fils[nbCommandes];
    for (int i = 0; i < nbCommandes; i++) {
    commandeStruct *cmd = cmds[i];
    if (cmd == NULL) {
            fprintf(stderr, "Erreur : commande %d invalide (NULL)\n", i);
            continue; // Passer à la commande suivante
        
    int ret; 
    pid_fils[i] = fork();
    if (pid_fils[i] == -1)
        {
            perror("fork");
            return 1;
        }

        if (pid_fils[i] == 0) // Code exécuté par les enfants
        {
            char *chemin = malloc(PATH_MAX);
            if (getcwd(chemin, PATH_MAX) == NULL)
            {
                perror("getcwd");
                free(chemin);
                return 1;
            }
            ret = fsh(chemin, &dernier_exit, cmd);
            exit(ret);
        }
    }
    }

    int ret;
    int status;
    for (int i = 0; i < nbCommandes; i++)
    {
        waitpid(pid_fils[i], &status, 0);
        if (WIFEXITED(status))
        {
            ret = WEXITSTATUS(status);
        }
    }

    return ret;

    
}