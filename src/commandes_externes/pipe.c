#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <wait.h>
#include "../../utils/commande.h"
#include "../../utils/extern.h"

int cmdpipe(cmd_pipe *p)
{
    int n = p->nbCommandes;
    int tpipe[n - 1][2]; // 1 pipe pour chaque paire de commandes
    pid_t pid_fils[n];

    for (int i = 0; i < n; i++)
    {
        int p = pipe(tpipe[i]);
        if (i < 0)
        {
            perror("pipe");
            return 1;
        }
    }

    for (int i = 0; i < n; i++)
    {
        pid_fils[i] = fork();
        if (pid_fils[i] == -1)
        {
            perror("fork");
            return 1;
        }

        if (pid_fils[i] == 0) // Code exécuté par les enfants
        {
            if (i > 0)
            {                                        // Si ce n'est pas le 1er processus on redirige l'entrée
                dup2(tpipe[i - 1][0], STDIN_FILENO); // l'entrée standard est redirigé vers la l'entrée du pipe
            }

            if (i < n - 1)
            {                                       //Si ce n'est pas le dernier processus on redirige la sortie
                dup2(tpipe[i][1], STDOUT_FILENO); // la sortie standard est rédirigé vers la sortie du pipe
            }

            // Fermeture de tout les pipes (psq ils servent plus maintenant qu'on a fais les duplications)
            for (int j = 0; j < n - 1; j++)
            {
                close(tpipe[j][0]);
                close(tpipe[j][1]);
            }

            //Exécution des commandes
            if (p->commandes[i]->type == CMD_INTERNE)
            {
            }
            else if (p->commandes[i]->type == CMD_EXTERNE)
            {
                cmd_extern(p->commandes[i]);
            }
            exit(0);
        }
    }

    for (int i = 0; i < n - 1; i++)
    {
        close(tpipe[i][0]);
        close(tpipe[i][1]);
    }

    // Attendre la fin de tous les processus enfants
    for (int i = 0; i < n; i++)
    {
        waitpid(pid_fils[i], NULL, 0);
    }
}

int main()
{
    // Commande ls -l
    cmd_simple *ls = malloc(sizeof(cmd_simple));
    ls->type = CMD_EXTERNE;
    char *argls[] = {"ls", "-l", NULL};
    ls->args = argls;

    // Commande grep <pattern>
    cmd_simple *grep = malloc(sizeof(cmd_simple));
    grep->type = CMD_EXTERNE;
    char *arggrep[] = {"grep", "3", NULL}; // Remplacez "pattern" par le motif que vous voulez chercher
    grep->args = arggrep;

    // Commande wc -l
    cmd_simple *wc = malloc(sizeof(cmd_simple));
    wc->type = CMD_EXTERNE;
    char *argwc[] = {"wc", "-l", NULL};
    wc->args = argwc;

    // Tableau de commandes
    cmd_simple *cmd[] = {ls, grep, wc};

    // Structure cmd_pipe
    cmd_pipe *p = malloc(sizeof(cmd_pipe));
    p->type = PIPE;
    p->nbCommandes = 3;
    p->commandes = cmd;

    // Exécution des commandes
    cmdpipe(p);

    // Libération de la mémoire allouée
    free(ls);
    free(grep);
    free(wc);
    free(p);

    return 0;
}