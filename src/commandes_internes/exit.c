#include <stdio.h>
#include <stdlib.h>


extern int dernier_exit ; 

int commande_exit(char* valeur){
    int code = valeur ? atoi(valeur) : dernier_exit;
    return code;
}
