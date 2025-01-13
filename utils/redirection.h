#ifndef REDIRECTION_H
#define REDIRECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include "commande.h"

/**
 * Redirige la sortie (ou l'entrée) de la commande vers un fichier spécifié avec écrasement ou en ajout.
 * @param cmd structure cmd_redirection qui stocke la commande a exécuté le type de la redirection et le fichier.
 * @return 0 si la commande est exécutée avec succès, 1 sinon.
 */

int redirection(cmd_redirection *cmd);
int redirection(cmd_redirection *cmd);

#endif