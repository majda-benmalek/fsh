#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "../../utils/commande.h"
#include "../../utils/gestionStruct.h"
#include "../../utils/freeStruct.h"
#include "../../utils/gestion.h"



/*TO DO : refaire le ARG_mAX*/

int decoupe_commande(char** commande , commandeStruct ** cmds , int * nbCmds , int maxCmds){
    //if(*nbCmds >= maxCmds) return -1;
    commandeStruct *cmdstruct = malloc(sizeof(commandeStruct));
    if (cmdstruct == NULL) {
        perror("Erreur allocation cmdStruct dans decoupe_commande");
        return -1;
    }
    if (remplissage_cmdStruct(CMD_STRUCT, NULL, NULL, NULL, NULL, NULL, NULL, 0, cmdstruct) == NULL) {
        perror("Erreur lors du remplissage de cmdStruct");
        free(cmdstruct); 
        return -1;
    }
    gestion_cmd(commande, cmdstruct);
    cmds[*nbCmds] = cmdstruct;
    (*nbCmds)++;
    return *nbCmds;
}

int decoupe_args(char** args , commandeStruct** cmds , int maxcmds){
    size_t taille = tailleArgs(args);
    int debut = 0;
    int nbcommandes = 0;
    while (debut < (int)taille)
    {
        int fin = debut;
        int imbrication = 0;
        bool estDansBloc = false;
        // trouver la fin du bloc 
        while (args[fin] != NULL){
            if(strcmp(args[fin], "{") == 0){
                imbrication++;
                estDansBloc = true;
            }else if(strcmp(args[fin], "}") == 0){
                imbrication--;
                if(imbrication == 0) estDansBloc = false;
            }else if (!estDansBloc && strcmp(args[fin], ";") == 0)
            {
                break;
            }
            fin++; 
        }
        int commandeSize = fin - debut + 1;
        char** commande = malloc(commandeSize * sizeof(char *));
        if(commande == NULL) {
            perror("Erreur alocation commande dans decoupe_args");
            return -1;
        }
        // extraire la commande entre debut et fin 
        if(arg_cmdsimple(args , commande , fin , debut) != 0){
            perror("arg_cmdsimple");
            for (int i = 0; commandeSize-1; i++) {
            free(commande[i]);
            }
            free(commande);
            return -1; 
        }

       
        


       
        if (decoupe_commande(commande, cmds, &nbcommandes, maxcmds) < 0) {
            for (int i = 0; i < commandeSize - 1; i++) { 
                free(commande[i]);
            }
            free(commande);
            return -1;
        }
    
        
        for (int i = 0; i < commandeSize - 1; i++) { 
            free(commande[i]);
        }
        free(commande);

        debut = fin + 1; 
    }

    //cmds[nbcommandes] = NULL;

    return nbcommandes;
}

