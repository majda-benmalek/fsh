#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>


//  CMD 2> FIC : redirection de la sortie erreur standard de la commande cmd sur le fichier (ordinaire, tube nommé...) FIC sans écrasement (ie, échoue si FIC existe déjà)


// * CMD 2>> FIC : redirection de la sortie erreur standard de la commande CMD sur le fichier (ordinaire, tube nommé...) FIC en concaténation


int redirection_erreur(char *instruction){
    int fd;
    char *file=NULL;
    const char* separateur = " ";
    char *cmd[1000];
    ssize_t i=0;
    int append=0;

    char *token = strtok(instruction,separateur);

    while (token !=NULL){
        if (strcmp(token,"2>")==0){
            file = strtok(NULL,separateur);
        }
        else if (strcmp(token,"2>>")==0){
            file = strtok(NULL,separateur);
            append = 1;
        }
        else{
            cmd[i]=token;
            i++;
        }
        token = strtok(NULL,separateur);
    }

    cmd[i]=NULL;

    if (file!=NULL){
        if (append){
            fd = open(file, O_WRONLY | O_CREAT | O_APPEND,0664);
        }else{
            fd = open(file,O_WRONLY | O_CREAT | O_EXCL, 0664);
        }
        if (fd<0){
            perror("erreur ouverture du fichier");
            return 1;
        }
    }
    else{
        perror(" pas de nom de fichier ");
        return 1;
    }

    int sortie_erreur_ancienne = dup(STDERR_FILENO);
    if (sortie_erreur_ancienne<0){
        perror("erreur dans la duplication de stderr");
        return 1;
    }

    int d= dup2(fd,STDERR_FILENO);
    if (d<0){
        perror("erreur de dup2");
        return 1;
    }

    d = close(fd);
    
    if (fd<0){
        perror("erreur lors de close");
    }

    if (cmd[0] != NULL){
        switch (fork()){
            case -1 :
            perror("erreur lors du fork");
            return 1;
            case 0 :
            if (execvp(cmd[0],cmd)<0){
                perror("execv erreur ");
                exit(1);
            }
            default :
                wait(NULL);
                d = dup2(sortie_erreur_ancienne,STDERR_FILENO);
                if (d<0){
                    perror("erreur defaut dup 2");
                    return 1;
                }
                d = close(sortie_erreur_ancienne);
                if (d<0){
                    perror("erreur close descrpiteur sortie erreur");
                }
        }


    }
    else{
        perror("execution commande");
        return 1;
    }
    return 0;
}