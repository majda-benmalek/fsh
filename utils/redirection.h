#ifndef REDIRECTION_H
#define REDIRECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include "commande.h"

/**
 * Redirige la sortie de la commande vers un fichier spécifié avec ecrasement ou en append.
 * @param input l'entrée de l'utilisateur
 * La fonction sépare la chaîne input en parties en tenant compte des espaces , puis elle extrait la commande et ses option qu'elle met dans un tableau d'argument ainsi que le fichier de redirection .
 * Ensuite elle exécute la commande spécifié et la redirige vers le fichier spécifié en tenant compte de l'option de redirection.
 * @return 0 si la commande est exécutée avec succès, 1 sinon.
 */

int redirection(cmd_redirection *cmd);

#endif