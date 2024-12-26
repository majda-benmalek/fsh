#ifndef SIGAUX_H
#define SIGAUX_H
#include <signal.h>

extern volatile sig_atomic_t sigint_received;
void configuration_signaux(sigset_t *block_mask);
void debloquer_signaux(sigset_t *block_mask);
void restaurer(sigset_t *block_mask);
void sigint_handleur(int sig);
void signaux_fils();
void sigaux_main();
void ignorer_sigterm();
#endif 