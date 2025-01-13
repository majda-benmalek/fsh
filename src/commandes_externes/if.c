#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include "../../utils/redirection.h"
#include "../../utils/gestion.h"
#include "../../utils/extern.h"
#include "../../utils/ftype.h"
#include <linux/limits.h>
#include "../../utils/commande.h"
#include "../../utils/freeStruct.h"
#include "../../utils/exit.h"
#include "../../utils/gestionStruct.h"

int exec_if(cmdIf *cmd, char *chemin)
{
    // redirection de la sortie d'erreur et de la sortie standard OU l'exécuté sur un autre processus
    int pid_enf = fork();
    switch (pid_enf)
    {
    case -1:
        perror("fork");
    case 0: // code du fils
        int ret = 1;
        if (cmd->test != NULL)
        {
            ret = fsh(chemin, &dernier_exit, cmd->test);
            exit(ret);
        }
    default: // code du père
        // recuperer le code de retour du fils
        int status;
        if (waitpid(pid_enf, &status, 0) == -1)
        {
            perror("waitpid");
            return 1;
        }
        int r = 0;
        if (status == 0)
        {
            if (cmd->commandeIf != NULL)
            {
                r = fsh(chemin, &dernier_exit, cmd->commandeIf);
                return r;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            if (cmd->commandeElse != NULL)
            {
                r = fsh(chemin, &dernier_exit, cmd->commandeElse);
                return r;
            }
            else
            {
                return 1;
            }
        }
    }
    return 1;
}