#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#include <sys/wait.h>
#include "../../utils/commande.h"
#include "../../utils/signaux.h"
#include "../../utils/signaux.h"

int cmd_extern(cmd_simple *cmd){
    

    if(cmd-> type != CMD_EXTERNE){
        write(2, "Commande externe attendue\n", strlen("Commande externe attendue\n"));
        return -4 ; 
    }
    if(cmd == NULL || cmd->args == NULL){
        write(2, "Commande invalide\n", strlen("Commande invalide\n\n"));
        return -4;
    } 
    



    int child_pid;
    int status = 0;


    switch (child_pid = fork())
    {
    case -1:
        perror(" erreur lors du fork");
        return -4;
        
    case 0 :
        signaux_fils();
    
        if(execvp(cmd->args[0] , cmd->args) == -1){
            perror("redirect_exec");
            exit(1);
        }
        break;

    default:
        pid_t pid = waitpid(child_pid, &status, 0);
        if (pid == -1) {
                if(sigint_received){
                    return -255;
                }
            };
        if(sigint_received){
            return -255;
        }
        if(WIFSIGNALED(status)){
            if(WTERMSIG(status) == SIGINT){
                sigint_received = 1;
                return -255;
            }
            return -255;
        }
        
        else if (WIFEXITED(status))
        {
            int exit_status = WEXITSTATUS(status);
            return exit_status;
        }
        break;
    }
    return 0 ;  
}