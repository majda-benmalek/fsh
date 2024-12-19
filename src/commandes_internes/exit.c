#include <stdio.h>
#include <stdlib.h>

extern int dernier_exit;

int commande_exit(char *valeur)
{
    if (valeur != NULL)
    {
        return atoi(valeur);
    }
    else
    {
        return dernier_exit;
    }
}
