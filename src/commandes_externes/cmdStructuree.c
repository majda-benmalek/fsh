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
#include "../../utils/cd.h"
#include "../../utils/signaux.h"


int execCmdStruct(commandeStruct **cmds, int nbCommandes ,char * chemin) {
   
    if (cmds == NULL || nbCommandes <= 0) {
        perror("tableau de commandes invalide");
        return -1;
    }
    int last_ret = -1;
    for (int i = 0; i < nbCommandes; i++) {
        commandeStruct *cmd = cmds[i];
        if (cmd == NULL) {
            perror("commande invalide");
            return -1; 
        }
         if (cmd->type == CMD_INTERNE && cmd->cmdSimple && strcmp(cmd->cmdSimple->args[0], "cd") == 0) {
            last_ret = fsh(chemin,&dernier_exit,cmd);
            continue;
        }
            int retour = fsh(chemin, &dernier_exit, cmd);
       last_ret = retour;
    }
   

    return last_ret;
}


