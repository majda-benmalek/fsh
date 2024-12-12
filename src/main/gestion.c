#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include "../../utils/cd.h"
#include "../../utils/pwd.h"
#include "../../utils/exit.h"
#include "../../utils/redirection.h"
#include "../../utils/prompt.h"
#include <stdbool.h>
#include "../../utils/gestion.h"
#include "../../utils/extern.h"
#include "../../utils/for.h"
#include "../../utils/ftype.h"
#include "../../utils/commande.h"
#include "../../utils/gestionStruct.h"
#define ARG_MAX 512

/*gerer les autres commandes */ 
// * code qui était 
    // char*args[ARG_MAX] = {NULL};
    // int nb_args= 0 ; 
    // char *token = strtok(input," \t") ; // pour gerer le cas ou l'utilisateur separe les arguments avec tab
    // while(token && nb_args < ARG_MAX-1){
    //     args[nb_args++] = token;
    //     token = strtok(NULL," \t");
    // }


void gestion_cmd(char **args,commandeStruct *cmdstruct){
    if(!cmdstruct){
        perror("Erreur Structure");
        return;
    }  
    // * là

    else if (strcmp(args[0],"\0") == 0 || strcmp(args[0],"\n") == 0 || strcmp(args[0]," ") == 0 ||strcmp(args[0],"\t") == 0){
            return;
    }

    if (strcmp(args[0],"for") == 0 ){
        cmdstruct ->cmdFor = make_for(args);
    }
    cmdstruct->cmdSimple = remplissage_cmdSimple(args);
    if(!cmdstruct->cmdSimple){
        perror("Erreur cmdSimple");
    }
}


int fsh(char *input, char *chemin, int *dernier_exit , commandeStruct *cmdstruct){
    int ret = 0;

    /*gestion de la commande Simple pour l'instant cad CMD_INTERNE && CMD_EXTERNE*/
    if(!cmdstruct || !cmdstruct->cmdSimple || !cmdstruct->cmdSimple->args[0]){
        perror("Structure commande");
        return -1 ;
    }
    
    if (cmdstruct->type == FOR){
            ret = boucle_for(cmdstruct->cmdFor);
            if (ret != 0)
            {
                perror("boucle_for");
                return ret;
            };
    }
    //exit
    if(cmdstruct->cmdSimple->type == CMD_INTERNE){
        char* cmd = cmdstruct->cmdSimple->args[0];
        // char premierchar = cmdstruct->cmdSimple->args[0][0];
        char * arg = cmdstruct->cmdSimple->args[1];
        if(strcmp(cmd,"exit") == 0){
            *dernier_exit = commande_exit(arg);
            if (*dernier_exit == -5)
        {
            ret = -5;
            *dernier_exit = 0;
            if(input) free(input);
            if(chemin) free(chemin);
            /*free(cmdstruct->cmdSimple->args);
            free(cmdstruct->cmdSimple);
            free(cmdstruct);*/
            freeCmdStruct(cmdstruct);
            return ret;
        }
        if(input) free(input);
        if(chemin) free(chemin);
        /*free(cmdstruct->cmdSimple->args);
        free(cmdstruct->cmdSimple);
        free(cmdstruct);*/
        freeCmdStruct(cmdstruct);
        exit(*dernier_exit);

        }

        // gestion de cd 
        else if(strcmp(cmd,"cd") == 0){
            ret = cd_commande(arg);
            if (getcwd(chemin, PATH_MAX) == NULL)
            {
                perror("getcwd");
                return 1;
            }
            return ret;
         }

         // gestion de pwd

        else if(strcmp(cmd,"pwd") == 0){
            ret = pwd();
        }
        //* Redirection > et >>
        else if (strstr(input, ">>") || strstr(input, ">"))
        {
            ret = redirection(input);
            if (ret != 0)
            {
                perror("Redirection");
                return ret;
            };
        }
        else if (strcmp(cmd,"ftype") == 0)
        {
            ret = ftype(arg);
            if (ret > 0)
            {
                perror("ftype");
                return ret;
            }
        }
        // else if (premierchar == '\0'|| premierchar == '\n' ||premierchar== ' ' || premierchar == '\t')
        // {
        //     ret = *dernier_exit;
        // }
    }else{
        ret = cmd_extern(cmdstruct->cmdSimple);
        if (ret < 0)
        {
            return ret;
        }
    }
    return ret;
}






/*void gestion_cmd(char *input, char *arg, char *cmd)
{
    char *espace = strchr(input, ' ');
    int indice_espace = espace - input;
    arg[0] = '\0';
    cmd[0] = '\0';
    if (input[0] == '\0')
    {
        sprintf(cmd, "%c", '\0');
    }
    if (espace != NULL && input[indice_espace + 1] != '\0') // ya un espace et un argument
    {
        snprintf(arg, strlen(input) - indice_espace, "%s", input + indice_espace + 1);
    }
    if (espace != NULL && strlen(arg) == 0) // ya un espace mais pas d'argument
    {
        snprintf(cmd, strlen(input), "%s", input); // un espace et sans argument
    }
    if (espace == NULL)
    {
        snprintf(cmd, strlen(input) + 2, "%s", input); //pas d'espace et pas d'argument
    }
    if (strlen(arg) >= 1)
    {
        snprintf(cmd, (strlen(input) - strlen(arg)), "%s", input); //pas d'espace apres l'argument
    }
}*/







// if (strcmp(token,"for") == 0){
//         cmdstruct->cmdFor=malloc(sizeof(cmdFor));
//         int ret = make_for(input,cmdstruct->cmdFor);
//         if (ret != 0){
            
//             perror("make for");
//             return;
//         };
//     } 



//         cmdstruct->cmdSimple->args = malloc(1);//TODO A CHANGER MAYBE MAIS LE PB C EST QU IL EST PAS ALLOUE


    /*cmdstruct->cmdSimple->args = realloc( cmdstruct->cmdSimple->args , sizeof(char*) * (nb_args +1));
    cmdstruct->cmdSimple->args[nb_args] = NULL;
    if(cmdstruct->cmdSimple->args[0]){
        if(strcmp(cmdstruct->cmdSimple->args[0], "exit") == 0 || 
        strcmp(cmdstruct->cmdSimple->args[0], "cd") == 0 ||
        strcmp(cmdstruct->cmdSimple->args[0], "pwd") == 0 ||
        strcmp(cmdstruct->cmdSimple->args[0], "ftype") == 0
        ){
            cmdstruct->cmdSimple->type = CMD_INTERNE;
        }else{
            cmdstruct->cmdSimple->type = CMD_EXTERNE;
        }
    }*/

    



/*int fsh(char *cmd, char *arg, char *input, char *chemin, int *dernier_exit)
{
    int ret = 0;
    if (strcmp(cmd, "exit") == 0)
    {

        *dernier_exit = commande_exit(arg);
        if (*dernier_exit == -5)
        {
            ret = -5;
            *dernier_exit = 0;
            return ret;
        }
        if (input != NULL)
        {
            free(input);
        }
        if (chemin != NULL)
        {
            free(chemin);
        }
        if (arg != NULL)
        {
            free(arg);
        }
        if (cmd != NULL)
        {
            free(cmd);
        }
        exit(*dernier_exit);
    }
    // Commande cd
    else if (strcmp(cmd, "cd") == 0)
    {
        ret = cd_commande(arg);
        if (getcwd(chemin, PATH_MAX) == NULL)
        {
            perror("getcwd");
            return 1;
        }
        return ret;
    }
    // Commande pwd
    else if (strcmp(cmd, "pwd") == 0)
    {
        ret = pwd();
    }
    // Redirection > et >>
    else if (strstr(input, ">>") || strstr(input, ">"))
    {
        ret = redirection(input);
        if (ret != 0)
        {
            perror("Redirection");
            return ret;
        };
    }
    else if (strcmp(cmd, "ftype") == 0)
    {
        ret = ftype(arg);
        if (ret > 0)
        {
            perror("ftype");
            return ret;
        }
    }
    else if (strstr(input, "for"))
    {
        ret = boucle_for(input);
        if (ret != 0)
        {
            perror("boucle_for");
            return ret;
        };
    }
    else if (cmd[0] == '\0' || cmd[0] == '\n' || cmd[0] == ' ' || cmd[0] == '\t')
    {
        ret = *dernier_exit;
    }
    else
    {
        ret = cmd_extern(input);
        if (ret < 0)
        {
            return ret;
        }
    }
    return ret;
}*/