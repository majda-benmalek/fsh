#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>


int redirection(char* input){
    char * args[1000]; // le tableau d'argument qu'on va utiliser dans excvp 
    char * filename = NULL ; 
    int fd ; 
    int i = 0 ; 
    const char* separateur = " " ;
    int append = 0 ;
    // ls -l cmd > f1
    char *token = strtok(input, separateur);
    // token -> ls

    /*on va commencer par separer la chaine input selon les espaces pour extraire le fichier vers lequel redirigier + la commande à executer*/

    while(token != NULL){
        if(strcmp(token,">")==0){  // token -> f1
            filename = strtok(NULL, separateur); //filname = f1
        }
        else if(strcmp(token,">>")==0){
            filename = strtok(NULL, separateur); //filname = f1
            append = 1 ; 
        }
        else{
            args[i] = token; /* [ls , -l] ici si c'est pas > ou >> on prend cmd et ses option */
            i++; 
        }

        // lire ce qu'il y a aprés 
        token = strtok(NULL,separateur);

    }
    args[i] = NULL ;

    if(filename != NULL){
        if(append){
            fd = open(filename , O_WRONLY|O_CREAT|O_APPEND, 0644);
        }else{
            fd = open(filename , O_WRONLY|O_CREAT|O_EXCL, 0644);
        }
        if(fd<0){
            perror("erreur lors de l'ouverture du fichier");
            return 1;
        }
    }else{
        perror("erreur fichier non specifié");
        return 1;
    }

    // garder le df de la sortie standard pour la rétablir aprés 
    int sortie_avant = dup(STDOUT_FILENO);  // comme si sortie_avant = STDOUT_FILENO 

    if(sortie_avant < 0){
        perror("dup");
        return 1; 
    }
    // faire en sorte que STDOUT_FILENO pointe vers le fichier fd
    dup2(fd,STDOUT_FILENO); // comme si STDOUT_FILENO = fd 
    close(fd);

    // creer un fils qui va executer la commande 
    if(args[0] != NULL){

    switch (fork()){
        case -1:
            perror("erreur lors de la creation du fils");
            return 1 ; 
        case 0 :
            if(execvp(args[0] , args) < 0){
                perror("execvp"); 
                exit(1);
            }
        default :
            wait(NULL); // attends son fils
            //retablir la sortie
            if(filename != NULL){
                if(dup2(sortie_avant, STDOUT_FILENO)<0){
                    perror("dup2");
                    close(sortie_avant);
                    return 1;
                }
                close(sortie_avant);
            }
    }
}else{
    perror("erreur lors de l'execution de la commande ");
}

return 0;

}