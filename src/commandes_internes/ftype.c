#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "../../utils/tarutils.h"
#define BUFFERSIZE 1024

int ftype(char *name)
{
    struct stat st;
    char *buf = malloc(BUFFERSIZE);
    if (buf == NULL)
    {
        perror("problème avec l'allocation de memoire ");
        goto error;
    }
    if (lstat(name,&st)!=0)
        {
            goto error;
        }
     switch (st.st_mode & S_IFMT) 
     {
        case S_IFREG : strcat(buf,"regular file\n");break;
        case S_IFDIR : strcat(buf,"directory\n"); break;
        case S_IFIFO : strcat(buf,"named pipe\n"); break;
        case S_IFLNK : strcat(buf,"symbolic link\n"); break;
        case S_IFCHR : strcat(buf,"other\n");break;
        default :buf="\n";
    }

    int res = write(1, buf, strlen(buf));
    if (res < 1)
    {
        perror("erreur write ");
        goto error;
    }

    return 0;

error:
    free(buf);
    return 1;
}
