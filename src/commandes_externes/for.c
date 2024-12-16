#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include "../../utils/redirection.h"
#include "../../utils/gestion.h"
#include "../../utils/extern.h"
#include "../../utils/ftype.h"
#include <linux/limits.h>
#include "../../utils/commande.h"
#include "../../utils/freeStruct.h"
#include "../../utils/exit.h"
#define ARG_MAX 512

void nouveau(char *ancienne, char *nouveau, commandeStruct *cmd)
{
    if (cmd->type == CMD_EXTERNE || CMD_INTERNE)
    {
        int k = 0;
        while (cmd->cmdSimple->args[k] != NULL)
        {
            char *a_changer = strstr(cmd->cmdSimple->args[k], ancienne);
            if (a_changer == NULL)
            {
                k++;
            }
            else
            {
                // printf("a_changer = %s\n",a_changer);
                int len_prefixe = strlen(cmd->cmdSimple->args[k]) - strlen(a_changer);
                // printf("len_prefixe = %d\n",len_prefixe);
                // char *postfixe  = cmd->cmdSimple->args[k]+strlen(ancienne);
                char *postfixe;
                if (strlen(a_changer) + strlen(ancienne) == strlen(cmd->cmdSimple->args[k]))
                {
                    printf("a_changer = %s\n", a_changer);
                    postfixe = cmd->cmdSimple->args[k] + strlen(a_changer) + strlen(ancienne);
                }
                else
                {
                    postfixe = cmd->cmdSimple->args[k] + strlen(ancienne);
                }

                if (strcmp(postfixe, "") == 0)
                {
                    postfixe = NULL;
                }
                char *prefixe[len_prefixe + 1];
                int len_final = strlen(nouveau);
                if (len_prefixe > 0)
                {
                    strncpy(prefixe, cmd->cmdSimple->args[k], len_prefixe);
                    len_final += len_prefixe;
                }
                cmd->cmdSimple->args[k] = NULL;
                int len_postfixe = 0;
                if (postfixe != NULL)
                {
                    len_postfixe = strlen(postfixe);
                    // printf("postfixe = %s\n",postfixe);
                    len_final += len_postfixe;
                    // printf("nouveau = %s\n",nouveau);
                }
                char *realloue = realloc(cmd->cmdSimple->args[k], len_final + 2);
                cmd->cmdSimple->args[k] = realloue;

                if (len_prefixe > 0)
                {
                    // printf("a_changer = %s\n",a_changer);
                    printf("postfixe = %s\n", postfixe);
                    printf("prefixe = %s\n", prefixe);
                    printf("nouveau =%s\n", nouveau);
                    printf("ancien = %s\n", ancienne);
                    // printf("cmdSimple->args[%d] = %s\n",k,cmd->cmdSimple->args[k]);
                    strcpy(cmd->cmdSimple->args[k], prefixe);
                    strcat(cmd->cmdSimple->args[k], nouveau);
                }
                else
                {
                    strcpy(cmd->cmdSimple->args[k], nouveau);
                    // printf("cmd->cmdSimple->args[%d] = %s\n",k,cmd->cmdSimple->args[k]);
                }
                if (len_postfixe > 0)
                {
                    strcat(cmd->cmdSimple->args[k], postfixe);
                }
                k++;
            }
        }
    }
}

void restaurer_var(char *ancienne, char *nouveau, commandeStruct *cmd)
{
    switch (cmd->type)
    {
    case CMD_EXTERNE:
        int k = 0;
        while (cmd->cmdSimple->args[k] != NULL)
        {
            char *var_str = strstr(cmd->cmdSimple->args[k], ancienne);
            if (strcmp(cmd->cmdSimple->args[k], ancienne) == 0)
            {
                cmd->cmdSimple->args[k] = NULL;
                cmd->cmdSimple->args[k] = malloc(strlen(nouveau) + 2);
                strcpy(cmd->cmdSimple->args[k], "$");
                strcat(cmd->cmdSimple->args[k], nouveau);
            }
            else if (var_str != NULL)
            { // TODO FAIRE LA MM PR LE RESTE
                char *reste = strstr(cmd->cmdSimple->args[k], ancienne);
                reste += strlen(ancienne);
                char *realloue = realloc(cmd->cmdSimple->args[k], strlen(nouveau) + 2); // PR 0 ET $
                if (realloue == NULL)
                {
                    perror("realloc");
                    return;
                }
                cmd->cmdSimple->args[k] = realloue;
                strcpy(cmd->cmdSimple->args[k], "$");
                strcat(cmd->cmdSimple->args[k], nouveau);
                strcat(cmd->cmdSimple->args[k], reste);
            }
            k++;
        }
        break;
    default:
        break;
    }
}

int optionA(struct dirent *entry, cmdFor *cmdFor)
{
    return (rechercheDansArgs("-A", cmdFor->op) && entry->d_name[0] == '.' && entry->d_name[1] != '.' && entry->d_name[1] != '\0');
}
// TODO changer la fct en int car void pas ouf si pb de malloc realloc tt ça faut renvoyer un int qui dis que y'a un pb

// for i in rep { ls -l $i ; echo $i ; }

// TODO Si ca ce passe mal ft faire un truc
int boucle_for(cmdFor *cmdFor)
{
    // int dernier_exit = 0; // ! pas besoin dernier exit est une var globale dans exit.h
    int ret = 0; // TODO A CHANGER;
    DIR *dir = opendir(cmdFor->rep);
    if (dir == NULL)
    {
        // perror("Erreur d'ouverture du repertoire");
        // perror("commande for run");
        fprintf(stderr, "command_for_run: %s\n", cmdFor->rep);
        ret = 1;
        return ret;
    }
    // ! for i in . -A { ftype $i }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // printf("dans le while\n");
        if (entry->d_name[0] != '.' || optionA(entry, cmdFor))
        {
            printf("entry->d_name = [%s] \n", entry->d_name);
            //? ici
            int nbr_cmd = 0;
            while (cmdFor->cmd[nbr_cmd] != NULL)
            {
                char *inter = malloc(strlen(cmdFor->variable) + 2); // ? CA C PR AVOIR LE BON NOM DE VARIABLE +2 pr $ et le char 0
                strcpy(inter, "$");
                strcat(inter, cmdFor->variable);
                // printf("inter = %s\n",inter);

                char *path = malloc(strlen(entry->d_name) + strlen(cmdFor->rep) + 2); // +2 pr / et '\0'
                if (path == NULL)
                {
                    return 1;
                }
                strcpy(path, cmdFor->rep);
                strcat(path, "/");
                strcat(path, entry->d_name);
                // printf("path = %s\n",path);

                nouveau(inter, path, cmdFor->cmd[nbr_cmd]);

                // change_var(entry->d_name,cmdFor->variable, cmdFor->cmd[nbr_cmd],cmdFor->rep);
                char *chemin = malloc(PATH_MAX);
                if (getcwd(chemin, PATH_MAX) == NULL)
                {
                    perror("getcwd");
                    free(chemin);
                    return 1;
                }

                if (cmdFor->cmd[nbr_cmd]->cmdSimple != NULL)
                {
                    for (int i = 0; cmdFor->cmd[nbr_cmd]->cmdSimple->args[i] != NULL; i++)
                    {
                        printf("cmdFor->cmd[%d]->args[%d] = %s\n", nbr_cmd, i, cmdFor->cmd[nbr_cmd]->cmdSimple->args[i]);
                    }
                }
                ret = fsh(chemin, &dernier_exit, cmdFor->cmd[nbr_cmd]);
                if (cmdFor->cmd[nbr_cmd] == NULL)
                {
                    perror("pb ds le changement de var");
                    free_for(cmdFor);
                    return 1;
                }

                char *ancienne = malloc(strlen(entry->d_name) + strlen(cmdFor->rep) + 2);
                strcpy(ancienne, cmdFor->rep);
                strcat(ancienne, "/");
                strcat(ancienne, entry->d_name);
                restaurer_var(ancienne, cmdFor->variable, cmdFor->cmd[nbr_cmd]);
                if (cmdFor->cmd[nbr_cmd] == NULL)
                {
                    perror("pb ds le changement de var");
                    free_for(cmdFor);
                    return 1;
                }
                if (inter != NULL)
                    free(inter);
                if (path != NULL)
                    free(path);
                if (ancienne != NULL)
                    free(ancienne);
                if (chemin != NULL)
                    free(chemin);
                nbr_cmd = nbr_cmd + 1;
            }
        }
    }
    closedir(dir);

    return ret;
}

//! Ancienne version

// #include <stdio.h>
// #include <stdlib.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/wait.h>
// #include <dirent.h>
// #include <errno.h>
// #include <limits.h>
// #include "../../utils/redirection.h"
// #include "../../utils/gestion.h"
// #include "../../utils/extern.h"
// #include "../../utils/ftype.h"
// #include <linux/limits.h>
// #define ARG_MAX 512

// int boucle_for(char *input)
// {
//     /**
//      * for i in rep { ls -l $i ; echo $i ; }
//      * 1- ectraire la variable i
//      * 2- extraire rep
//      * 3- extraire les commande ls -l $i ; echo $i ;
//      * 4- pour chaque commande remplacer $i par le nom du fichier
//      * 5- utiliser strok pour separer les commandes avec le separateur " ;"
//      * 6- pour chaque commande verifier si c'est une redirection donc appeler la fonction redirection
//      * sinon utiliser execvp pour executer la commande
//      */

//     // pour la variable i
//     char var;
//     char *debut_variable = input + 4;
//     char *fin_variable = strstr(debut_variable, " in");
//     int ret = 0;

//     if (fin_variable != NULL)
//     {
//         int len = fin_variable - debut_variable;
//         if (len != 1)
//         {
//             perror("Erreur de syntaxe, la variable doit contenir un seul caractère");
//             return 1;
//         }
//         var = *debut_variable;
//     }
//     else
//     {
//         perror("in attendu");
//         ret=1;
//         return ret;
//     }
//     // extraction de rep
//     // for i in rep { ls -l $i ; echo $i ; }
//     // rep avant le " {" et aprés le " in"

//     char *debut_rep = strstr(input, "in ");
//     char *fin_rep = strstr(input, " {");
//     char *fin_cmd = strstr(input, " }");

//     if (fin_rep == NULL || fin_cmd == NULL || debut_rep == NULL)
//     {
//         perror("Erreur de syntaxe");
//         ret=1;
//         return ret;
//     }

//     debut_rep += 3;
//     int len_rep = fin_rep - debut_rep;
//     char rep[len_rep + 1];
//     strncpy(rep, debut_rep, len_rep);
//     rep[len_rep] = '\0';

//     // extraction des commandes
//     char *debut_cmd = fin_rep + 2;
//     int len_cmd = fin_cmd - debut_cmd;
//     char commandes[len_cmd + 1];
//     strncpy(commandes, debut_cmd, len_cmd);
//     commandes[len_cmd] = '\0';
//     // printf("cmd = %s\n var = %c\n rep = %s\n", commandes, var, rep);

//     // remplacer les $i

//     DIR *dir = opendir(rep);
//     if (dir == NULL)
//     {
//         perror("Erreur d'ouverture du repertoire");
//         ret=1;
//         return ret;
//     }
//     struct dirent *entry;
//     while ((entry = readdir(dir)) != NULL)
//     {

//         // pour ignorer les fichiers cachés
//         if (entry->d_name[0] != '.')
//         {
//             // printf("Fichier trouvé : %s\n", entry->d_name);
//             char commandes_modifiee[len_cmd + 1000];
//             strcpy(commandes_modifiee, commandes);
//             /* maintenant que j'ai le nom du fichier je vais chercher dans les commande si je trouve des occurence de $i pour ça je vais utiliser un autre pointeur ui va parcourir la liste*/
//             char *ptr = commandes_modifiee;
//             while ((ptr = strchr(ptr, '$')) != NULL)
//             {
//                 // verifier si c'est bien i
//                 if (*(ptr + 1) == var)
//                 {
//                     // deplacer la portion aprés la variable de pos + strlen d->d_name
//                     /*deplacer la chaine qui se situe apres la varible donc aprs pos+2 ($i) de longueur strlen(pos+2) donc ca nous donne le nombre de caractére a deplacer enfin les deplacer vers la position pos+strlen(d->d_name)*/
//                     char chemin[1000];

//                     snprintf(chemin, sizeof(chemin), "%s/%s", rep, entry->d_name);
//                     memmove(ptr + strlen(chemin), ptr + 2, strlen(ptr + 2) + 1);
//                     // puisque c'est deplacer on va copier
//                     memcpy(ptr, chemin, strlen(chemin));
//                     ptr += strlen(chemin);
//                 }
//                 else
//                 {
//                     ptr++;
//                 }
//             }
//             //  séparer les cmd avec le separateur ;
//             //  appliquer chaque commande
//             char *cmd = strtok(commandes_modifiee, ";");
//             char *cmd1 = cmd;
//             snprintf(cmd, strlen(cmd1), "%s", cmd1 + 1);

//             while (cmd != NULL)
//             {
//                 char *arg1 = malloc(ARG_MAX);
//                 char *cmd1 = malloc(ARG_MAX);
//                 gestion_cmd(cmd, arg1, cmd1);
//                 int dernier_exit = 0;
//                 char chemin_fsh[4096] = "";
//                 if (getcwd(chemin_fsh, 4096) == NULL)
//                 {
//                     perror("Erreur de getcwd");
//                     closedir(dir);
//                     free(arg1);
//                     free(cmd1);
//                     ret=1;
//                     return ret;
//                 }

//                 ret = fsh(cmd1, arg1, cmd, chemin_fsh, &dernier_exit);
//                 if (ret < 0)
//                 {
//                     perror("Erreur de fsh");
//                     closedir(dir);
//                     free(arg1);
//                     free(cmd1);
//                     return ret;
//                 }
//                 cmd = strtok(NULL, ";");
//             }
//         }
//     }

//     closedir(dir);

//     return ret;
// }
