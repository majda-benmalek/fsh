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

int exec_if(cmdIf *cmd)
{
    // redirection de la sortie d'erreur et de la sortie standard OU l'exécuté sur un autre processus
    int pid_enf = fork();
    switch (pid_enf)
    {
    case -1:
        perror("fork");
    case 0: // code du fils

    default: // code du père
}