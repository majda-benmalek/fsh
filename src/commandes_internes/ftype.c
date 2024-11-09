#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "../../utils/tarutils.h"
#define BUFFERSIZE 1024

int ftype(char *name){
    struct stat st;
    char *buf=malloc(BUFFERSIZE);
    if (buf==NULL)
    {
        perror("problème avec l'allocation de memoire ");
        goto error;
    }
    if (lstat(name,&st)!=0)
        {
            perror("Nom invalide ");
            goto error;
        }
    buf=name;
     switch (st.st_mode & S_IFMT) 
     {
        case S_IFREG : strcat(buf," : fichier ordinaire\n");break;
        case S_IFDIR : strcat(buf," : repertoire\n"); break;
        case S_IFIFO : strcat(buf," : tube\n"); break;
        case S_IFLNK : strcat(buf," : lien\n"); break;
        case S_IFCHR : strcat(buf," : fichier special\n");break;
        default :buf=" : autre\n";
    }
    
    int res=write(1,buf,strlen(buf));
    if (res<1){
        perror("erreur write ");
        goto error;
    }
    
    return 1;
    
    error:
        free(buf);
        return 0;

}