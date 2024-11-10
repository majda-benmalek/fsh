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
#include <errno.h>
#include <dirent.h>

/**
 * Retourne le nom du répertoire courant.
 *
 * @return Un pointeur vers une chaîne de caractères contenant le nom du
 * répertoire courant. Retourne NULL en cas d'erreur.
 */
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

/**
 * Construit le chemin absolu du répertoire courant.
 *
 * Cette fonction utilise une approche récursive pour construire le chemin
 * absolu du répertoire courant en remontant jusqu'à la racine du système
 * de fichiers.
 *
 * @param path Un pointeur vers une chaîne de caractères où le chemin absolu
 * sera stocké.
 * @param size La taille du buffer pointé par `path`.
 * @return Un pointeur vers une chaîne de caractères contenant le chemin
 * absolu du répertoire courant. Retourne NULL en cas d'erreur.
 */
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

/**
 * Affiche le chemin absolu du répertoire courant.
 *
 * Cette fonction utilise `chemin_absolu` pour obtenir le chemin absolu du
 * répertoire courant et l'affiche sur la sortie standard.
 */
int pwd()
{
    char *full_path = malloc(PATH_MAX);
    if (full_path == NULL)
    {
        perror("malloc");
        return 0;
    }
    full_path = chemin_absolu(full_path, PATH_MAX);
    fprintf(stdout, "%s\n", full_path);
    chdir(full_path); // au cas ou pendant le parcours un changement de rep a était effectué on reviens
                      // au rep courant
    if (full_path == NULL)
    {
        perror("pwd");
        return 0;
    }
    else
    {
        free(full_path);
    }
    return 1;
}