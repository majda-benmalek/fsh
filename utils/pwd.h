#ifndef PWD_H
#define PWD_H

#include <sys/types.h>
#include <dirent.h>

// Déclare la fonction nom_du_repertoire
char *nom_du_repertoire();
char* pwd();
char * chemin_absolu(char *path, size_t size);

#endif // PWD_H