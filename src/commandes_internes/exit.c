#include <stdio.h>
#include <stdlib.h>


extern int dernier_exit ; 

void commande_exit(char* valeur){
    int code = valeur ? atoi(valeur) : dernier_exit;
    printf("Exit avec le code %d\n", code);
    exit(code);

}