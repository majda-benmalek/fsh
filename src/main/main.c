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
#include "../../utils/for.h"


int dernier_exit = 0 ; // pour initialiser la derniére valeur de retour 

int main()
{
    char *input;
    char *chemin = malloc(100);
    getcwd(chemin, 100);
     if (chemin == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        printf("\033[0;32m %s", chemin);
        input = readline("$ \033[0m");
        
        add_history(input);

        if(strncmp(input, "exit", 4) == 0){
            char* valeur_code ; 
            if(input[4] == ' '){
                // si j'ai un espace aprés exit donc l'utilisateur a fournis un code 
                valeur_code = input + 5 ; 

            }else{
                valeur_code = NULL ; 
            }
            commande_exit(valeur_code);
            input = NULL ; 
        } // pour le exit si on met une autre valeur que le 0 le makefile affiche une erreur ce qui esr normal mais y'a possiblité de changer ca (demander si c'est nécessaire)
        else if (strncmp(input, "cd", 2) == 0)
        {
            char *chemin_cd = input + 3; //? pas besoin de le free psq il pointe vers input qui est lui meme un pointeur qui vas etre libérer a un moment 
            cd_commande(chemin_cd);
            getcwd(chemin, 512);
            input = NULL;
        }
        else if (strcmp(input, "ls") == 0)
        {
            if (fork() == 0)
            {
                execlp(input, input, NULL);
                perror("execlp");
                exit(EXIT_FAILURE);
            };
            wait(NULL);
            input = NULL;
        }
        else if (strcmp(input, "pwd") == 0)
        {
            char *path = nom_du_repertoire();
            printf("%s\n", path);
            input = NULL;
            free(path);
        }
        else if(strstr(input , ">>") || strstr(input , ">")){
            //printf("detection de > >> \n");
            int result = redirection(input);

            if(result != 0){
                printf("Redirection échouée\n");
                input = NULL ;
            }

            input = NULL;
        }else if(strncmp(input, "for", 3) == 0){
            int result = boucle_for(input);
            if(result != 0){
                printf("Boucle for échouée\n");
                input= NULL;
            }
            input = NULL;
        }
        else
        {
            printf("Commande non reconnue : %s\n", input);
            input = NULL;
           
        }
    }

    free(chemin);
    free(input);
    return 0;
}