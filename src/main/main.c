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

bool ok = true;
int main()
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
    if (chdir(getenv("HOME"))==-1)
    {
        perror("chdir");
        exit(EXIT_FAILURE);
    }
    if (getcwd(chemin, PATH_MAX) == NULL)
    {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }
    int nbr_cmd = 6;
    char **cmd = malloc(nbr_cmd * sizeof(char *));
    cmd[0] = "ls";
    cmd[1] = "cd";
    cmd[2] = "pwd";
    cmd[3] = "exit";
    cmd[4] = ">>";
    cmd[5] = ">";

    while (1)
    {
        rl_outstream = stderr;
        prompt(chemin, input, ok);
        ok = true;
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
            char *chemin_cd = input + 3; //? pas besoin de le free psq il pointe vers input qui est lui meme un pointeur qui vas etre libérer a un moment
            ok=!(cd_commande(chemin_cd));
            getcwd(chemin, 512);
        }
        //* Commande pwd
        else if (strcmp(input, "pwd") == 0 || strcmp(input, "pwd ") == 0)
        {
            ok=pwd();
        }
        //* Redirection > et >>
        else if (strstr(input, ">>") || strstr(input, ">"))
        {
            // printf("detection de > >> \n");
            int result = redirection(input);

            if (result != 0)
            {
                printf("Redirection échouée\n");
            }
        }
        //* Commande externe ls sans argument
        else if (strcmp(input, "ls") == 0)
        {
            if (fork() == 0)
            {
                int r = execlp("ls", "ls", NULL);
                if (r == -1)
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
            fprintf(rl_outstream, "Commande non reconnue : %s\n", input);
            ok=false;
        }
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