#define _DEFAULT_SOURCE 
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
        perror("stat .");
        goto error;
    }

    parent = opendir("..");
    if (parent == NULL)
    {
        perror("opendir ..");
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
                perror("readdir");
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
            perror("lstat");
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
        perror("strdup");
        goto error;
    }
    closedir(parent);
    return ret;

error:
    if (parent != NULL)
    {
        closedir(parent);
    }
    if (ret != NULL)
    {
        free(ret);
    }
    return NULL;
}

char *chemin_absolu(char *path, size_t size)
{
    char *dir_name = nom_du_repertoire();
    if (dir_name == NULL)
    {
        return path;
    }

    if (strcmp(dir_name, "/") == 0)
    {
        free(dir_name);
        return strdup("/");
    }

    if (chdir("..") != 0)
    {
        free(dir_name);
        return NULL;
    }

    char current_path[PATH_MAX];
    getcwd(current_path, sizeof(current_path));
    if (strcmp(current_path, "/") == 0)
    {
        snprintf(path, size, "/%s", dir_name);
        free(dir_name);
        return strdup(path);
    }

    // Récursion pour obtenir le chemin du parent
    char *parent_path = chemin_absolu(path, size);
    if (parent_path == NULL)
    {
        free(dir_name);
        return NULL;
    }

    snprintf(path, size, "%s/%s", parent_path, dir_name);
    free(dir_name);
    free(parent_path);

    return strdup(path);
}

char *pwd()
{
    char path[PATH_MAX] = "";
    return chemin_absolu(path, sizeof(path));
}