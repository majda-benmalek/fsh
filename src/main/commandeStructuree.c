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
#define ARG_MAX 400
#define MAX_CMDS 100

/*TO DO : refaire le ARG_mAX*/


int decoupe_commande(char** commande , commandeStruct ** cmds , int * nbCmds , int maxCmds){
    if(*nbCmds >= maxCmds) return -1;
    commandeStruct * cmdstruct = malloc(sizeof(commandeStruct));
    if(cmdstruct == NULL) {
        perror("Erreur alocation cmdStruct dans decoupe_commande");
        return -1;
    }
    cmdstruct = remplissage_cmdStruct(CMD_STRUCT, NULL, NULL, NULL, NULL, NULL, NULL, 0, cmdstruct); 
    gestion_cmd(commande, cmdstruct);
    cmds[(*nbCmds) ++] = cmdstruct;
    return 0 ;
}

int decoupe_args(char** args , commandeStruct** cmds , int maxcmds){

    size_t taille = tailleArgs(args);
    int debut = 0;
    int nbcommandes = 0;


    while (debut < taille)
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
        char** commande = malloc(fin - debut + 1 * sizeof(char*));
        if(commande == NULL) {
            perror("Erreur alocation commande dans decoupe_args");
            return -1;
        }

        // extraire la commande entre debut et fin 
        if(arg_cmdsimple(args , commande , fin , debut) != 0){
            perror("arg_cmdsimple");
            for (int i = 0; commande[i] != NULL; i++) {
            free(commande[i]);
            }
            free(commande);
            return -1; 
        }
        decoupe_commande(commande , cmds , &nbcommandes , maxcmds);
        for (int i = 0; commande[i] != NULL; i++) {
            free(commande[i]);
        }
        free(commande);
        debut = fin + 1;
    }

    return nbcommandes;
}


