#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include "../../utils/redirection.h"
#include "../../utils/gestion.h"
#include "../../utils/extern.h"
#include "../../utils/ftype.h"
#include <linux/limits.h>
#include "../../utils/commande.h"
#include "../../utils/freeStruct.h"
#include "../../utils/exit.h"
#define ARG_MAX 512

int compte_occ(char *chaine, char * sous_chaine){
    int res = 0;
    char *c = strstr(chaine,sous_chaine);
    while (c!= NULL){
        c+=strlen(sous_chaine);
        res++;
        c=strstr(c,sous_chaine);
    }
    return res;
}

void nouveau(char *ancienne,char *nouveau, commandeStruct *cmd){ 
    if (cmd->type == CMD_EXTERNE || CMD_INTERNE){
        int k = 0;
        while (cmd->cmdSimple->args[k]!= NULL){
            char *ancienne_cmd = strdup(cmd->cmdSimple->args[k]);
            char *a_changer = strstr(ancienne_cmd,ancienne);
            if (a_changer == NULL){
                k++;
            }
            else{
                int occ_ancienne = compte_occ(cmd->cmdSimple->args[k],ancienne);
                int taille = strlen(cmd->cmdSimple->args[k]) - occ_ancienne*strlen(ancienne) + occ_ancienne*strlen(nouveau) + 1;
                char *realloue = realloc(cmd->cmdSimple->args[k],taille); //TODO TEst = null
                cmd->cmdSimple->args[k] = realloue;      
                char *prefixe = ancienne_cmd;
                cmd->cmdSimple->args[k][0] = '\0'; // pour pas qu'il soit à null
                while (a_changer !=NULL){
                    int taille_prefixe = strlen(prefixe)-strlen(a_changer);
                    if (taille_prefixe>0){
                        strncat(cmd->cmdSimple->args[k],prefixe,taille_prefixe);
                    }
                    strcat(cmd->cmdSimple->args[k],nouveau);
                    prefixe = a_changer + strlen(ancienne);
                    a_changer = strstr(prefixe,ancienne);
                }
                strcat(cmd->cmdSimple->args[k], prefixe);
                k++;
            }
        }
    }
}

//TODO Si ca ce passe mal ft faire un truc
int boucle_for(cmdFor *cmdFor){
    int dernier_exit=0; //TODO A CHANGER
    int ret=0; //TODO A CHANGER;
    DIR *dir = opendir(cmdFor->rep);
    if (dir == NULL)
    {
        fprintf(stderr, "command_for_run: %s\n", cmdFor->rep);
        ret = 1;
        return ret;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL){
        if (entry->d_name[0] != '.')
        {            //? ici
            int nbr_cmd = 0;
            while (cmdFor->cmd[nbr_cmd] != NULL)
            {   
                char * inter = malloc(strlen(cmdFor->variable)+2);// ? CA C PR AVOIR LE BON NOM DE VARIABLE +2 pr $ et le char 0
                strcpy(inter,"$");
                strcat(inter,cmdFor->variable);
    
                char * path = malloc(strlen(entry->d_name)+strlen(cmdFor->rep)+2);// +2 pr / et '\0'
                if (path == NULL){
                    return 1;
                }
                strcpy(path,cmdFor->rep);
                strcat(path,"/");
                strcat(path,entry->d_name);
                nouveau(inter,path,cmdFor->cmd[nbr_cmd]);
                
                ret = fsh("",&dernier_exit,cmdFor->cmd[nbr_cmd]);
                if (cmdFor->cmd[nbr_cmd] == NULL){
                    perror("pb ds le changement de var");
                    free_for(cmdFor);
                    return 1;
                }
                
                char * ancienne = malloc(strlen(entry->d_name)+strlen(cmdFor->rep)+2);
                strcpy(ancienne,cmdFor->rep);
                strcat(ancienne,"/");
                strcat(ancienne,entry->d_name);
                char * dollar=malloc(strlen(cmdFor->variable)+2);// ? CA C PR AVOIR LE BON NOM DE VARIABLE +2 pr $ et le char 0
                strcpy(dollar,"$");
                strcat(dollar,cmdFor->variable);
                nouveau(ancienne,dollar,cmdFor->cmd[nbr_cmd]);
                if (cmdFor->cmd[nbr_cmd] == NULL){
                    perror("pb ds le changement de var");
                    free_for(cmdFor);
                    return 1;
                }
                nbr_cmd=nbr_cmd+1;
            }
        }
    }
    closedir(dir);

    return ret;
}