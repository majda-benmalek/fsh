#include <stdio.h>
#include <stdlib.h>

extern int dernier_exit;

int commande_exit(char *valeur)
{
    if (valeur[0] != '\0')
    {
        return atoi(valeur);
    }
    else
    {
        return dernier_exit;
    }
}
