#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "../../utils/commande.h"
#include "../../utils/gestion.h"
#define ARG_MAX 512


commandeStruct *remplissage_cmdStruct(Type type , cmd_simple * cmdSimple , cmd_pipe* pipestruct , cmdIf* cmdIfStruct , cmdFor * cmdForStruct , int nbcommandes ){

    commandeStruct *cmd = malloc(sizeof(commandeStruct));

    cmd->type = type;
    cmd->cmdSimple = cmdSimple;
    cmd->pipe = pipestruct;
    cmd->cmdIf = cmdIfStruct;
    cmd->cmdFor = cmdForStruct;
    cmd->nbCommandes = nbcommandes;

    return cmd;
}

void freeCmdSimple(cmd_simple* cmd){
    if(!cmd) return ;
  
        if(cmd->args){
            for ( char**arg = cmd->args ; *arg ; ++arg ){
                free(*arg);
            }
            free(cmd->args);
        }
        free(cmd);
}


void freePipe(cmd_pipe* pipeStruct){
    if(!pipeStruct) return ;
        for(int i = 0 ; i < pipeStruct->nbCommandes ; i++){
            free(pipeStruct->commandes[i]);
        }
        free(pipeStruct->commandes);
        free(pipeStruct);
}

void freeCmdStruct(commandeStruct* cmd){
    if(!cmd ) return;
    if(cmd->cmdSimple){
        freeCmdSimple(cmd->cmdSimple);
    }

    if(cmd->pipe){
        freePipe(cmd->pipe);
    }
    // if(cmd->cmdIf){
    //     if(cmd->cmdIf->commandeIf){
    //         for(int i = 0 ; cmd->cmdIf->commandeIf[i] != NULL ; i++){
    //             freeCmdStruct(cmd->cmdIf->commandeIf[i]);
    //         }
    //         free(cmd->cmdIf->commandeIf);
    //     }
    //     if(cmd->cmdIf->commandeElse){
    //         for(int i = 0 ; cmd->cmdIf->commandeElse[i] != NULL ; i++){
    //             freeCmdStruct(cmd->cmdIf->commandeElse[i]);
    //         }
    //         free(cmd->cmdIf->commandeElse);
    //     }
    //     if(cmd->cmdIf->test){
    //         freePipe(cmd->cmdIf->test);
    //     }

    //     free(cmd->cmdIf);
    // }

    // if(cmd->cmdFor){
    //     if(cmd->cmdFor->op){
    //         for(char**op = cmd->cmdFor->op ; *op ; ++op){
    //             free(*op);
    //         }
    //         free(cmd->cmdFor->op);
    //     }
    //     if(cmd->cmdFor->cmd){
    //         for(int i = 0 ; i < cmd->cmdFor->nbCommandes ; i++){
    //             freeCmdStruct(cmd->cmdFor->cmd[i]);
    //         }
    //         free(cmd->cmdFor->cmd);
    //     }
    //     free(cmd->cmdFor->rep);
    //     free(cmd->cmdFor);
    // }

    free(cmd);

}




cmd_simple *remplissage_cmdSimple(char ** args ){
    cmd_simple *cmd = malloc(sizeof(cmd_simple));

    if(!cmd){
        perror("malloc CommandSimple");
        return NULL;
    }

    int nbargs = 0;
    while(args[nbargs]){nbargs++;}
    cmd->args = malloc((nbargs+1) * sizeof(char*));
    if(!cmd->args){
        perror("malloc args");
        free(cmd);
        return NULL;
    }

    for (int i = 0 ; i < nbargs; i++) {
        cmd->args[i] = strdup(args[i]);
        if(!cmd->args[i]){
            perror("strdup");
            for(int j = 0 ; j < i ; j++){
                free(cmd->args[j]);
            }
            free(cmd->args);
            free(cmd);
            return NULL;
        }
    }

    cmd->args[nbargs]  = NULL;

    if(strcmp(args[0] , "cd") == 0 || strcmp(args[0] , "pwd") == 0 || strcmp(args[0] , "ftype") == 0 || strcmp(args[0] , "exit") == 0 ) {
        cmd->type = CMD_INTERNE;
    }
    else if(strstr(args[0] , ">") ||strstr(args[0] , ">>")  || strstr(args[0] , "2>") || strstr(args[0] , "2>>") || strstr(args[0] , "<") || strstr(args[0] , ">|") ){
        cmd->type = REDIRECTION;
    }else{
        cmd->type = CMD_EXTERNE;
    }
    return cmd;
}


size_t tailleArgs(char **args)
{
    size_t taille = 0;
    while (args[taille] != NULL)
    {
        taille++;
    }
    return taille;
}


// // type
// //   char* rep;
// //   char variable ; 
// int nbCommandes;
// commandeStruct** cmd;
// //  op

cmdFor* make_for(char ** args){
    cmdFor *cmdFor = malloc(sizeof(cmdFor));
    // cmdFor *cmdFor = malloc(ARG_MAX);
    if (!cmdFor){
        perror("problème d'allocation de mémoire pour for");
        return NULL;
    }
    //TODO j'ai commenté
    if (tailleArgs(args) < 8){
        perror("problème de syntaxe");
        printf("la taille de l'argument = %d\n",tailleArgs(args));
        for (size_t i = 0; i<tailleArgs(args);i++){
            printf("%s\n",args[i]);
        }
        return NULL;
    }

    //* --------- option----------
    // cmdFor->op = NULL;

    // ? -------- Type ---------
    cmdFor->type = FOR;

    // * ------------------ variable ---------------
    if (strlen(args[1]) != 1){
        perror("Erreur de syntaxe, la variabme doit contenir un seul caractère");
        return NULL;
    }
    cmdFor->variable= *args[1];
    cmdFor->rep = args[4];
    // cmdFor->op=malloc(sizeof(char*)*8);
    cmdFor->op=malloc(ARG_MAX*8);
    // ? ----------------- option-----------
    int i = 5;
    int j = 0;
    while (strcmp(args[i],"{") != 0 ){
        if (strcmp(args[i],"-A") == 0 || strcmp(args[i],"-r") == 0 ){
            cmdFor->op[j]=args[i];
            i++;
            j++;
        }
        else if(strcmp(args[i],"-e") == 0 || strcmp(args[i],"-t") == 0 || strcmp(args[i],"-p") == 0 ){
            if (args[i+1][0]!='-'){
                cmdFor->op[j]=args[i];
                i=i+2;
                j++;
            }
            else{
                perror("il manque un argument");
                return NULL;
            }
        }
        //TODO j'ai commenté
        // else{
        //     perror("ce n'est pas un argument valide");
        //     return NULL;
        // }
    }
    if (strcmp(args[i],"{") == 0){
         i++; // pour sauter l'{
    }
    char * tab[ARG_MAX];
    unsigned int k;
    while (args[i]!= NULL){
        tab[k]=args[i];
        k++;
        i++;
    }
    gestion_cmd(tab,*(cmdFor->cmd));
    return cmdFor;
}



    // char *debut_variable = input + 4;
    // char *fin_variable = strstr(debut_variable, " in");
    // int ret = 0;

    // if (fin_variable != NULL)
    // {
    //     int len = fin_variable - debut_variable;
    //     if (len != 1)
    //     {
    //         perror("Erreur de syntaxe, la variable doit contenir un seul caractère");
    //         return 1;
    //     }
    //     cmdFor->variable = *debut_variable;
    // }
    // else
    // {
    //     perror("in attendu");
    //     ret=1;
    //     // return ret;
    // }

    // // ? --------------- répertoire --------------
    // char *debut_rep_opt = strstr(input, "in ");
    // char *fin_rep_opt = strstr(input, " {");
    // char *fin_cmd = strstr(input, " }");

    // if (fin_rep_opt == NULL || fin_cmd == NULL || debut_rep_opt == NULL)
    // {
    //     perror("Erreur de syntaxe");
    //     ret=1;
    //     // return ret;
    // }

    // debut_rep_opt += 3;
    // int len_rep = fin_rep_opt - debut_rep_opt;
    // char rep[len_rep + 1];
    // strncpy(rep, debut_rep_opt, len_rep);
    // rep[len_rep] = '\0';
    
    // // * ------------ extraction des commandes ------------
    // char *debut_cmd = fin_rep_opt + 2;
    // int len_cmd = fin_cmd - debut_cmd;
    // char commandes[len_cmd + 1];
    // strncpy(commandes, debut_cmd, len_cmd);
    // commandes[len_cmd] = '\0';

    // // TODO APPPELER LA FCT FSH SUR COMMANDES
    // // gestion_cmd(commandes,cmdFor->cmd);
    // gestion_cmd(commandes, *(cmdFor->cmd));

    // return ret;
// }

