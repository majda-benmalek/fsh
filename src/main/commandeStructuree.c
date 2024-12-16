/*#define _DEFAULT_SOURCE
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

/*TO DO : refaire le ARG_mAX


int decoupe_commande(char* commande , commandeStruct ** cmds , int * nbCmds , int maxCmds){
    if(*nbCmds >= maxCmds) return -1;
    commandeStruct * cmdstruct = malloc(sizeof(commandeStruct));
    if(cmdstruct == NULL) {
        perror("Erreur alocation cmdStruct dans decoupe_commande");
        return -1;
    }
    cmdstruct = remplissage_cmdStruct(CMD_STRUCT, NULL, NULL, NULL, NULL, NULL, 0, cmdstruct); 

    char *args[ARG_MAX] = {NULL};
    int nb_args = 0;
    char *token = strtok(commande, " \t"); 
    while (token && nb_args < ARG_MAX - 1)
        {
            args[nb_args++] = token;
            token = strtok(NULL, " \t");
        }
    args[nb_args] = NULL;

    gestion_cmd(args, cmdstruct);
    cmds[(*nbCmds) ++] = cmdstruct;
    return 0 ;
}



int gestion_commandes_structures(char* input , commandeStruct** cmds , int max_cmd){
    int nbCommandes = 0 ; 
    char* debut = input;
    int inmbrication = 0 ; // 0 si {} mais si c'est pas fermé > 0 
    bool dansBloc = false ; 
    for(char* courent = debut ; *courent != '\0' ; courent++)
    {
        // tester si c'est pas un bloc
        if(*courent == '{'){
            inmbrication++ ;
            dansBloc = true ;
        }else if(*courent == '}'){
            inmbrication-- ;
            if(inmbrication == 0){
                dansBloc = false ;
            }

        }//si pas de bloc et ; alors on extrait la commande 
        else if(*courent == ';' && !dansBloc){
            //on extrait la commande
            *courent = '\0' ;
            // decouper la commande du debut jusuqu'à current ('\0')
            if(decoupe_commande(debut , cmds , &nbCommandes , max_cmd)<0){
                perror("decoupe commande dans gestion_commandes_structures");
                return -1 ;
            }
            // mettre debut a current + 1
            debut = courent + 1 ;
        }

    }
    // dernière commande qui n'a pas de ; 
    if(!dansBloc && *debut != '\0'){
        if(decoupe_commande(debut , cmds , &nbCommandes , max_cmd)<0){
                perror("decoupe commande dans gestion_commandes_structures");
                return -1 ;
            }
    }

    return nbCommandes ;
}


*/