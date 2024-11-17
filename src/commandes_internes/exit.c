#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern int dernier_exit ; 

int commande_exit(char* valeur){
    int code;
    if(strlen(valeur)>1){
        code=atoi(valeur);
    }else{
        code=dernier_exit;
    }
    return code;
}
