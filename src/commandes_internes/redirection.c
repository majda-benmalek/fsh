#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include "../../utils/commande.h"
#include "../../utils/gestionStruct.h"
#include "../../utils/gestion.h"
#define PATH_MAX 1024

int dterminer_flags(char* separateur){
    if(strcmp(separateur , ">>") == 0 ){
        return O_WRONLY | O_CREAT | O_APPEND;

    }else if(strcmp(separateur , ">") == 0){
        return O_WRONLY | O_CREAT | O_TRUNC;
    }else if(strcmp(separateur , "<") == 0){
        return O_RDONLY;
    }else{
        perror("separateur non reconnu");
        return -1;
    }
}

int ouvrir_fichier(const char *fichier , const char *separateur ){

    int flags = dterminer_flags(separateur);
    if(flags == -1){
        return -1;
    }
    int fd = open(fichier , flags ,0644);
    if(fd == -1){
        perror("erreur ouverture fichier");
        return -1;
    }
    return fd;
    
}

int sauvgarde_descripteurs (int *descripteur , int *copie){
    if(!copie){
        perror("pointeur null pour le copie");
        return -1;
    }
    *copie = dup(descripteur);
    if(*copie < 0){
        perror("erreur lors de la sauvegarde");
        return -1;
    }
    return 0;
}

int restauration_descipteur(int copie , int descripteur){
    if(dup2(copie,descripteur) < 0){
        perror("erreur lors de la restauration");
        close(copie);
        return -1;
    }
    close(copie);
    return 0;
}


int duplication_fd(int fd, char* separateur){
    int cible = -1;
    if(strcmp(separateur , ">>") == 0 || strcmp(separateur , ">") == 0){
        cible = STDOUT_FILENO;
    }else{
        cible = STDIN_FILENO;
    }

    if(dup2(fd,cible) < 0){
        perror("erreur duplication fd");
        return -1;
    }
    return 0;
}


int  redirection(cmd_redirection *cmdredirect){
    if(cmdredirect ==  NULL || cmdredirect->cmd == NULL || cmdredirect->fichier == NULL || cmdredirect->separateur == NULL)
    {
       perror("Arguments pour la redirection manquants");
       return 1;
    }

    char *fichier = cmdredirect->fichier;
    char *separateur = cmdredirect->separateur;

    int fd = ouvrir_fichier(fichier,separateur);
    if(fd < 0 ){
        perror("ouverture fichier");
        return -1;
    }
    int copie_stdout = -1 , copie_stdin = -1;
    if(sauvgarde_descripteurs(STDOUT_FILENO , &copie_stdout) < 0){
        perror("sauvgarde descripteurs");
        close(fd);
        return -1;
    }
    if(sauvgarde_descripteurs(STDIN_FILENO , &copie_stdin) < 0){
        perror("sauvgarde descripteurs");
        close(fd);
        return -1;
    }

    if(duplication_fd(fd ,separateur) < 0){
        perror("duplication fd");
        restauration_descipteur(copie_stdout, STDOUT_FILENO);
        restauration_descipteur(copie_stdin, STDIN_FILENO);
        close(fd);
        return -1;
    }
    close(fd);

    int dernier_exit = 0;
    char* chemin = malloc(PATH_MAX);
    if(chemin == NULL){
        perror("Erreur lors de l'allocation de mémoire");
        close(copie_stdin);
        close(copie_stdout);
        return 1;
    }
    if(getcwd(chemin,PATH_MAX)==NULL){
        perror("getcwd");
        free(chemin);
        close(copie_stdin);
        close(copie_stdout);
        return 1;
    }
    fsh(chemin , &dernier_exit, remplissage_cmdStruct(cmdredirect->cmd->type,cmdredirect->cmd,NULL,NULL,NULL,NULL,1,NULL));

    if(chemin) free(chemin);
    if (restauration_descipteur(copie_stdout, STDOUT_FILENO) < 0 ||
        restauration_descipteur(copie_stdin, STDIN_FILENO) < 0) {
        perror("restaurer descripteur");
        return -1;
    }

    return 0 ;



}
/*int redirection(cmd_redirection *cmdredirect){

    int inverse=0;
    int append = 0 ;

    int fd = -1  , copie_stdout = -1 , copie_stdin = -1;
    
    if (cmdredirect ==  NULL ) {
        perror("Structure redirection vide");
        return 1;
    }
    if(cmdredirect->cmd == NULL || cmdredirect->fichier == NULL || cmdredirect->separateur == NULL)
    {
       perror("Arguments pour la redirection manquants");
       return 1;
    }

    

    char *fichier = cmdredirect->fichier;
    printf("%s\n" , fichier);
    char *separateur = cmdredirect->separateur;

    if(strcmp(separateur , ">>") == 0)
    {
        append = 1;
    }
    
    else if(strcmp(separateur , "<") == 0)
    {
       inverse = 1;
    }

    if(append)
    {
        fd = open(fichier , O_WRONLY | O_CREAT | O_APPEND , 0644);
        if(fd<0){
        perror("Erreur lors de l'ouverture du fichier");
        return 1;
        }
    }
    else if(inverse)
    {
        printf("Tentative d'ouverture du fichier : %s\n", fichier);
        fd = open(fichier , O_RDONLY);
        if(fd<0){
        perror("Erreur lors de l'ouverture du fichier");
        return 1;
        printf("fichier ouvert\n");
    }
    }
    else{
        
        fd = open(fichier , O_WRONLY | O_CREAT | O_TRUNC , 0644);
        if(fd<0){
        perror("Erreur lors de l'ouverture du fichier");
        return 1;
        }
    }
    
    


    copie_stdout = dup(STDOUT_FILENO);
    copie_stdin = dup(STDIN_FILENO);

    if (copie_stdout < 0 || copie_stdin < 0) {
        perror("Erreur lors de la sauvegarde des descripteurs");
        close(fd);
        return 1;
    }


    if(strcmp(separateur , ">>") == 0 || strcmp(separateur , ">") == 0)
    {
       if(dup2(fd , STDOUT_FILENO) < 0){
        perror("Erreur lors de la duplication du fd");
        close(fd);
        close(copie_stdin);
        close(copie_stdout);
        return 1;
       } 
    }
    else if(strcmp(separateur , "<") == 0)
    {
        if(dup2(fd,STDIN_FILENO)<0){
            perror("Erreur lors de la duplication du fd");
            close(fd);
            close(copie_stdin);
            close(copie_stdout);
            return 1;
        }
    }

    close(fd);


    int dernier_exit = 0;

    char* chemin = malloc(PATH_MAX);
    if(chemin == NULL){
        perror("Erreur lors de l'allocation de mémoire");
        close(copie_stdin);
        close(copie_stdout);
        return 1;
    }
    if(getcwd(chemin,PATH_MAX)==NULL){
        perror("getcwd");
        free(chemin);
        close(copie_stdin);
        close(copie_stdout);
        return 1;
    }
    fsh(chemin , &dernier_exit, remplissage_cmdStruct(cmdredirect->cmd->type,cmdredirect->cmd,NULL,NULL,NULL,NULL,1,NULL));


     if (dup2(copie_stdout, STDOUT_FILENO) < 0) {
        perror("Erreur lors de la restauration de la sortie standard");
        close(copie_stdout);
        close(copie_stdin);
        return 1;
    }
    if (dup2(copie_stdin, STDIN_FILENO) < 0) {
        perror("Erreur lors de la restauration de l'entrée standard");
        close(copie_stdout);
        close(copie_stdin);
        return 1;
    }


    close(copie_stdin);
    close(copie_stdout);
    free(chemin);

    return 0;
    
}*/
