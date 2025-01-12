#ifndef SIGAUX_H
#define SIGAUX_H
#include <signal.h>

extern volatile sig_atomic_t sigint_received;

void sigint_handleur(int sig);
/**
 * @brief réinitialise les gestionnaires de signaux aux comportements par défaut pour un processus fils
 */

void signaux_fils();


/**
 * @brief Configure les gestionnaires de signaux pour le main
 * 
 * @details
 * - ignore `SIGTERM`
 * - Définit un gestionnaire pour le signal `SIGINT` afin de capturer son occurrence en définissant une variable globale sigint_received`.
 */
void sigaux_main();

#endif 