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

int dernier_exit = 0; // pour initialiser la derniére valeur de retour
int main(void)
{
    char *input = malloc(MAX_INPUT);
    if (input == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    char *chemin = malloc(PATH_MAX);
    if (chemin == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    //* Ouvert fsh directement dans le répertoire personnel de l'utilisateur (à voir)
    // if (chdir(getenv("HOME"))==-1)
    // {
    //     perror("chdir");
    //     exit(EXIT_FAILURE);
    // }

    if (getcwd(chemin, PATH_MAX) == NULL)
    {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }
    rl_outstream = stderr;
    int ok = 0;
    int *p_ok = &ok;
    while (1)
    {
        prompt(chemin, input, p_ok);
        ok = 0;
        //* Commande exit
        if (strncmp(input, "exit", 4) == 0)
        {
            char *valeur_code;
            if (input[4] == ' ')
            {
                // si j'ai un espace aprés exit donc l'utilisateur a fournis un code
                valeur_code = input + 5;
            }
            else
            {
                valeur_code = NULL;
            }
            commande_exit(valeur_code);
        } // pour le exit si on met une autre valeur que le 0 le makefile affiche une erreur ce qui esr normal mais y'a possiblité de changer ca (demander si c'est nécessaire)
        //* Commande cd
        else if (strncmp(input, "cd", 2) == 0)
        {
            // char *chemin_cd = input + 3; //? pas besoin de le free psq il pointe vers input qui est lui meme un pointeur qui vas etre libérer a un moment
            ok = cd_commande(input);
            getcwd(chemin, 512); // met le nouveau chemin dans la variable chemin
        }
        //* Commande pwd
        else if (strcmp(input, "pwd") == 0 || strcmp(input, "pwd ") == 0)
        {
            ok = pwd();
        }
        //* Redirection > et >>
        else if (strstr(input, ">>") || strstr(input, ">"))
        {
            // printf("detection de > >> \n");
            ok = redirection(input);

            if (ok != 0)
            {
                write(2, "Redirection échouée\n", strlen("Redirection échouée\n"));
            }
        }
        //* Commande externe ls sans argument
        else if (strcmp(input, "ls") == 0)
        {
            if (fork() == 0)
            {
                ok = execlp("ls", "ls", NULL);
                if (ok == -1)
                {
                    perror("execlp");
                    exit(EXIT_FAILURE);
                }
            }
            wait(NULL);
        }
        //* Pas une commande reconnue
        else
        {
            char *msg = malloc(MAX_INPUT);
            sprintf(msg, "Commande non reconnue : %s\n", input);
            write(2, msg, strlen(msg));
            ok = 1;
            if (msg != NULL)
            {
                free(msg);
            }
        } // Réinitialisation de ok à la fin de chaque itération
    }
    if (input != NULL)
    {
        free(input);
    }
    if (chemin != NULL)
    {
        free(chemin);
    }
    return 0;
}
