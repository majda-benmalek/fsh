#define _DEFAULT_SOURCE // SI YA UN PROBLEME D4INCLUDE A LA COMPILATION
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <libgen.h>
#include <sys/param.h>
#include "../../utils/tarutils.h"
#include <errno.h>
#include <dirent.h>



char *nom_du_repertoire()
{
    struct stat st_target, st_ent; // stat du rep dont on veut le nom et stat des entrées du rep courant
    DIR *parent = NULL;
    struct dirent *ent;
    char *ret = NULL;

    if (stat(".", &st_target) != 0)
    {
        goto error;
    }

    parent = opendir("..");
    if (parent == NULL)
    {
        goto error;
    }

    while (1)
    {
        errno = 0;
        ent = readdir(parent);
        if (ent == NULL)
        {
            if (errno != 0)
            {
                goto error;
            }
            else
            {
                goto error;
            }
        }
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
        {
            continue; // ignorer . et ..
        }

        char ent_path[PATH_MAX];

        snprintf(ent_path, PATH_MAX, "../%s", ent->d_name);

        if (lstat(ent_path, &st_ent) != 0)
        {
            goto error;
        }
        if (st_ent.st_ino == st_target.st_ino && st_ent.st_dev == st_target.st_dev)
        {
            break;
        }
    }

    ret = strdup(ent->d_name);
    if (ret == NULL)
    {
        goto error;
    }
    closedir(parent);
    return ret;

    error : 
    if (parent!=NULL)
    {
       closedir(parent);
    }
    if(ret!=NULL){
        free(ret);
    }
    return NULL;
}