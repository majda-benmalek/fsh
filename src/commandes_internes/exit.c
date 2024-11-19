#include <stdio.h>
#include <stdlib.h>


extern int dernier_exit ; 

int commande_exit(char* valeur){
    //int code =0;//= valeur ? atoi(valeur) : dernier_exit;
    // printf("dernier_exit = %d\n",dernier_exit);
    // printf("valeur = %s\n",valeur);
    // printf("valeur = %d\n",atoi(valeur));
    
    //TODO : vérifier si la valeur est un nombre
    
    if(valeur[0] != '\0'){
        printf("valeur = %d\n",atoi(valeur));
        return atoi(valeur);
    }else{
        printf("dernier_exit = %d\n",dernier_exit);
       return dernier_exit;
    }
    // printf("code = %d\n",code);
    // return code;
}
