#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h> // n'est pas reconnu jcp prk



int cd_commande(char * chemin ){
    /*garder le chemin courant pour l'utiliser comme précedent */
    if (strlen(chemin) > 0) {
        char last_char = chemin[strlen(chemin) - 1];
        if(last_char == ' '){
            chemin[strlen(chemin) - 1] = '\0';
        }
    }

    if(strcmp(chemin , "") == 0 || strcmp(chemin , " ") == 0 ){
        chemin = getenv("HOME"); // voir si c'est autorisé d'utiliser getenv 

        if(chemin == NULL){
            perror("Variable HOME non trouvée");
            return 1 ; 
        }
    }

    

    static char chemin_precedent[PATH_MAX] = "";
    char chemin_courant[PATH_MAX];


    if(getcwd(chemin_courant, sizeof(chemin_courant)) == NULL){ // doute sur ça 
        perror("chemin courant non trouvé ");
        return 1;
    }

    /*on va tester si le chemin a ete fournit si c'est pas le cas on retourne vers $HOME*/

    /*if(strcmp(chemin , "") == 0 || chemin == NULL){
        chemin = getenv("HOME"); // voir si c'est autorisé d'utiliser getenv 

        if(chemin == NULL){
            perror("Variable HOME non trouvée");
            return 1 ; 
        }

    }*/

    else if(strcmp(chemin , "-") == 0){
        chemin = chemin_precedent; 
        if(strlen(chemin) == 0){
            perror("aucun répertoire précedent n'a été trouvé");
            return 1 ; 
        }
    }

    if(chdir(chemin) < 0 ){
        perror("cd"); 
        return 1; 
    }

    strncpy(chemin_precedent, chemin_courant , sizeof(chemin_precedent)-1) ;

    chemin_precedent[sizeof(chemin_precedent)-1] = '\0' ;

    return 0 ; 

}

