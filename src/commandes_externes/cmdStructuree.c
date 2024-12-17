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
/*void afficherCommandeStruct(commandeStruct *cmd) {
    if (cmd == NULL) {
        printf("Commande : NULL\n");
        return;
    }

    printf("Commande de type : %d\n", cmd->type);

    switch (cmd->type) {
        case CMD_EXTERNE:
        case CMD_INTERNE:
            printf("  Commande simple :\n");
            if (cmd->cmdSimple) {
                printf("    Arguments : ");
                if (cmd->cmdSimple->args) {
                    for (char **arg = cmd->cmdSimple->args; *arg != NULL; arg++) {
                        printf("%s ", *arg);
                    }
                } else {
                    printf("(NULL)");
                }
                printf("\n");
            } else {
                printf("    NULL\n");
            }
            break;

        case PIPE:
            printf("  Commande pipe :\n");
            if (cmd->pipe) {
                printf("    Nombre de commandes : %d\n", cmd->pipe->nbCommandes);
                for (int i = 0; i < cmd->pipe->nbCommandes; i++) {
                    printf("    Commande %d :\n", i);
                    afficherCommandeStruct(cmd->pipe->commandes[i]);
                }
            } else {
                printf("    NULL\n");
            }
            break;

        case IF:
            printf("  Commande IF :\n");
            if (cmd->cmdIf) {
                printf("    Condition :\n");
                afficherCommandeStruct(cmd->cmdIf->test);

                printf("    Commandes IF :\n");
                for (int i = 0; cmd->cmdIf->commandeIf && cmd->cmdIf->commandeIf[i] != NULL; i++) {
                    afficherCommandeStruct(cmd->cmdIf->commandeIf[i]);
                }

                printf("    Commandes ELSE :\n");
                for (int i = 0; cmd->cmdIf->commandeElse && cmd->cmdIf->commandeElse[i] != NULL; i++) {
                    afficherCommandeStruct(cmd->cmdIf->commandeElse[i]);
                }
            } else {
                printf("    NULL\n");
            }
            break;

        case FOR:
            printf("  Commande FOR :\n");
            if (cmd->cmdFor) {
                printf("    Répertoire : %s\n", cmd->cmdFor->rep ? cmd->cmdFor->rep : "(NULL)");
                printf("    Variable : %s\n", cmd->cmdFor->variable ? cmd->cmdFor->variable : "(NULL)");

                printf("    Commandes dans le FOR :\n");
                for (int i = 0; cmd->cmdFor->cmd && cmd->cmdFor->cmd[i] != NULL; i++) {
                    afficherCommandeStruct(cmd->cmdFor->cmd[i]);
                }
            } else {
                printf("    NULL\n");
            }
            break;

        case REDIRECTION:
            printf("  Commande Redirection :\n");
            if (cmd->cmdRed) {
                printf("    Fichier : %s\n", cmd->cmdRed->fichier ? cmd->cmdRed->fichier : "(NULL)");
                printf("    Séparateur : %s\n", cmd->cmdRed->separateur ? cmd->cmdRed->separateur : "(NULL)");
                printf("    Commande associée :\n");
                afficherCommandeStruct((commandeStruct *)cmd->cmdRed->cmd);
            } else {
                printf("    NULL\n");
            }
            break;

        default:
            printf("  Type de commande inconnu ou non géré\n");
    }
}

// Fonction pour afficher un tableau de commandes structurées
void afficherTableauCommandes(commandeStruct **cmds, int nbCommandes) {
    if (cmds == NULL || nbCommandes <= 0) {
        printf("Tableau de commandes vide ou invalide.\n");
        return;
    }

    printf("Affichage du tableau de commandes (%d commandes) :\n", nbCommandes);
    for (int i = 0; i < nbCommandes; i++) {
        printf("\n--- Commande %d ---\n", i + 1);
        afficherCommandeStruct(cmds[i]);
    }
}*/



int execCmdStruct(commandeStruct **cmds, int nbCommandes) {
    //afficherTableauCommandes(cmds, nbCommandes);
  

    if (cmds == NULL || nbCommandes <= 0) {
        perror("tableau de commandes invalide");
        return -1;
    }
    pid_t pid_fils[nbCommandes];
    int last_ret = -1;
    for (int i = 0; i < nbCommandes; i++) {
        commandeStruct *cmd = cmds[i];
        if (cmd == NULL) {
            perror("commande invalide");
            return -1; 
        }
         if (cmd->type == CMD_INTERNE && cmd->cmdSimple && strcmp(cmd->cmdSimple->args[0], "cd") == 0) {
              char *chemin = malloc(PATH_MAX);
                if (getcwd(chemin, PATH_MAX) == NULL) {
                    perror("getcwd");
                    free(chemin);
                    exit(-1); 
                }
            last_ret = fsh(chemin,&dernier_exit,cmd);
            continue;
        }

        pid_fils[i] = fork();
        if (pid_fils[i] == -1) {
            perror("fork");
            return -1;
        }
        else if (pid_fils[i] == 0) { 
            char *chemin1 = malloc(PATH_MAX);
                if (getcwd(chemin1, PATH_MAX) == NULL) {
                    perror("getcwd");
                    free(chemin1);
                    exit(-1); 
                }
            int retour = fsh(chemin1, &dernier_exit, cmd);
            exit(retour); 
        }
        else {
            int status;
            pid_t pid = waitpid(pid_fils[i], &status, 0);
            if (pid == -1) {
                perror("waitpid");
                return -1;
            }
            if (WIFEXITED(status)) {
                last_ret = WEXITSTATUS(status); 
            } 
            
        }
    }
   

    return last_ret;
}


