#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h> 


int cd_commande(char * chemin ){
    /*garder le chemin courant pour l'utiliser comme précedent */

    static char chemin_precedent[PATH_MAX] = "";
    char chemin_courant[PATH_MAX];
    char chelin_test [PATH_MAX];

    ;
    if(getcwd(chemin_courant, sizeof(chemin_courant)) == NULL){ // doute sur ça 
        perror("chemin courant non trouvé ");
        return 1;
    }

    /*on va tester si le chemin a ete fournit si c'est pas le cas on retourne vers $HOME*/

    if(chemin == NULL){
        chemin = getenv("HOME"); // voir si c'est autorisé d'utiliser getenv 

        if(chemin == NULL){
            perror("Variable HOME non trouvée");
            return 1 ; 
        }

    }

    else if(strcmp(chemin , "-") == 0){
        chemin = chemin_precedent; 
        if(strlen(chemin) == 0){
            perror("aucun répertoire précedent n'a été trouvé");
            return 1 ; 
        }
    }

    if(chdir(chemin) <0 ){
        perror("Impossible de se déplacer dans le dossier"); 
        return 1; 
    }else{
        getcwd(chelin_test, sizeof(chelin_test));
       // printf("%s\n" , chelin_test); 
       //! je veux pas que ca affiche ou on est maintenant juste que ca déplace -MAJDA
    }

    strncpy(chemin_precedent, chemin_courant , sizeof(chemin_precedent)-1) ;

    chemin_precedent[sizeof(chemin_precedent)-1] = '\0' ;

    return 0 ; 

}

// ! pas le droit d'avoir 2 main 
// int main(int argc , char * argv[]){
//     char* chemin ; 

//     if(argc > 1){
//         chemin = argv[1];
//     }else{
//         chemin = NULL ; 
//     }

//     if(cd_commande(chemin) != 0){
//         perror("Echec");
//         return 1;
//     }
//     return 0;

// }
