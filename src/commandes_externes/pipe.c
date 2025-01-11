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
#include "../../utils/gestion.h"
#include "../../utils/gestionStruct.h"
#include "../../utils/exit.h"
#include "../../utils/freeStruct.h"

int cmdpipe(cmd_pipe *p)
{
    int n = p->nbCommandes;
    int tpipe[n - 1][2]; // 1 pipe pour chaque paire de commandes
    pid_t pid_fils[n];
    for (int i = 0; i < n; i++)
    {
        int p = pipe(tpipe[i]);
        if (p < 0)
        {
            perror("pipe");
            return 1;
        }
    }

    for (int i = 0; i < n; i++)
    {
        int ret;
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
            {                                     // Si ce n'est pas le dernier processus on redirige la sortie
                dup2(tpipe[i][1], STDOUT_FILENO); // la sortie standard est rédirigé vers la sortie du pipe
            }

            // Fermeture de tout les pipes (psq ils servent plus maintenant qu'on a fais les duplications)
            for (int j = 0; j < n - 1; j++)
            {
                close(tpipe[j][0]);
                close(tpipe[j][1]);
            }

            char *chemin = malloc(PATH_MAX);
            if (getcwd(chemin, PATH_MAX) == NULL)
            {
                perror("getcwd");
                free(chemin);
                return 1;
            }
            commandeStruct *cmd = remplissage_cmdStruct(p->commandes[i]->type, p->commandes[i], NULL, NULL, NULL,NULL, 0, NULL);
            ret = fsh(chemin, &dernier_exit, cmd);
            if (cmd != NULL)
                freeCmdStruct(cmd);
            exit(ret);
        }
    }

    // Fermeture de tout les pipes dans le père
    for (int i = 0; i < n - 1; i++)
    {
        close(tpipe[i][0]);
        close(tpipe[i][1]);
    }

    // TODO récupérer les valeurs de retour des fils
    //  Attendre la fin de tous les processus enfants
    // récupérer les valeurs de retour des fils
    int ret;
    int status;
    for (int i = 0; i < n; i++)
    {
        waitpid(pid_fils[i], &status, 0);
        if (WIFEXITED(status))
        {
            ret = WEXITSTATUS(status);
        }
    }

    return ret;
}
