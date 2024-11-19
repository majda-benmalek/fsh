#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include "../../utils/redirection.h"
#include "../../utils/gestion.h"
#include "../../utils/extern.h"
#include "../../utils/ftype.h"



int boucle_for(char *input)
{
    /**
     * for i in rep { ls -l $i ; echo $i ; }
     * 1- ectraire la variable i
     * 2- extraire rep
     * 3- extraire les commande ls -l $i ; echo $i ;
     * 4- pour chaque commande remplacer $i par le nom du fichier
     * 5- utiliser strok pour separer les commandes avec le separateur " ;"
     * 6- pour chaque commande verifier si c'est une redirection donc appeler la fonction redirection
     * sinon utiliser execvp pour executer la commande
     */

    // pour la variable i
    char var;
    char *debut_variable = input + 4;
    char *fin_variable = strstr(debut_variable, " in");

    if (fin_variable != NULL)
    {
        int len = fin_variable - debut_variable;
        if (len != 1)
        {
            perror("Erreur de syntaxe, la variable doit contenir un seul caractère");
            return 1;
        }
        var = *debut_variable;
    }
    else
    {
        perror("in attendu");
        return 1;
    }
    // extraction de rep
    // for i in rep { ls -l $i ; echo $i ; }
    // rep avant le " {" et aprés le " in"

    char *debut_rep = strstr(input, "in ");
    char *fin_rep = strstr(input, " {");
    char *fin_cmd = strstr(input, " }");

    if (fin_rep == NULL || fin_cmd == NULL || debut_rep == NULL)
    {
        perror("Erreur de syntaxe");
        return 1;
    }

    debut_rep += 3;
    int len_rep = fin_rep - debut_rep;
    char rep[len_rep + 1];
    strncpy(rep, debut_rep, len_rep);
    rep[len_rep] = '\0';

    // extraction des commandes
    char *debut_cmd = fin_rep + 2;
    int len_cmd = fin_cmd - debut_cmd;
    char commandes[len_cmd + 1];
    strncpy(commandes, debut_cmd, len_cmd);
    commandes[len_cmd] = '\0';
    // printf("cmd = %s\n var = %c\n rep = %s\n", commandes, var, rep);

    // remplacer les $i

    DIR *dir = opendir(rep);
    if (dir == NULL)
    {
        perror("Erreur d'ouverture du repertoire");
        return 1;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // ignorer le . et ..
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            // pour ignorer les fichiers cachés
            if (entry->d_name[0] != '.')
            {
                // printf("Fichier trouvé : %s\n", entry->d_name);
                char commandes_modifiee[len_cmd + 1000];
                strcpy(commandes_modifiee, commandes);
                /* maintenant que j'ai le nom du fichier je vais chercher dans les commande si je trouve des occurence de $i pour ça je vais utiliser un autre pointeur ui va parcourir la liste*/
                char *ptr = commandes_modifiee;
                while ((ptr = strchr(ptr, '$')) != NULL)
                {
                    // verifier si c'est bien i
                    if (*(ptr + 1) == var)
                    {
                        // deplacer la portion aprés la variable de pos + strlen d->d_name
                        /*deplacer la chaine qui se situe apres la varible donc aprs pos+2 ($i) de longueur strlen(pos+2) donc ca nous donne le nombre de caractére a deplacer enfin les deplacer vers la position pos+strlen(d->d_name)*/
                        char chemin[1000];

                        snprintf(chemin, sizeof(chemin), "%s/%s", rep, entry->d_name);
                        memmove(ptr + strlen(chemin), ptr + 2, strlen(ptr + 2) + 1);
                        // puisque c'est deplacer on va copier
                        memcpy(ptr, chemin, strlen(chemin));
                        ptr += strlen(chemin);
                    }
                    else
                    {
                        ptr++;
                    }
                }
                // printf("Commande modifiée : %s\n", commandes_modifiee);
                //  séparer les cmd avec le separateur ;
                //  appliquer chaque commande
                char *cmd = strtok(commandes_modifiee, ";");

                while (cmd != NULL)
                {
                    printf("Commande à exécuter après séparation : %s\n", cmd);
                    pid_t pid = fork();
                    switch (pid)
                    {
                    case -1:
                        perror("Erreur de fork");
                        return 1;
                    case 0:
                        if (strstr(cmd, ">>") || strstr(cmd, ">"))
                        {
                            // printf("detection de > >> \n");
                            int result = redirection(cmd);
                            if (result != 0)
                            {
                                // printf("Redirection échouée\n");
                            }
                        }else if(strstr(cmd , "ftype")){
                            int result = ftype(entry->d_name);
                        }
                        else
                        {
                            char *args[100];
                            int i = 0;
                            char *token = strtok(cmd, " ");

                            while (token != NULL)
                            {
                                args[i] = token;
                                // printf("Argument %d : %s\n", i, args[i]);
                                token = strtok(NULL, " ");
                                i++;
                            }
                            args[i] = NULL;
                            /*printf("Arguments pour execvp :\n");
                            for (int j = 0; j < i; j++) {
                                printf("%s ", args[j]);
                            }
                            printf("\n");*/
                            if (execvp(args[0], args) < 0)
                            {
                                perror("Erreur d'exécution");
                                return 1;
                            }
                        }
                        break;

                    default:
                        waitpid(pid, NULL, 0);
                        // printf("Commande terminée : %s\n", cmd);
                    }
                    // fsh(cmd,)
                    cmd = strtok(NULL, ";");
                }
            }
        }
    }

    closedir(dir);

    return 0;
}

/*int main(){
    char input[] = "for x in /home/moi/sabrina { ls -l $x ; echo $x ; }" ;

    boucle_for(input);
    return 0;
}*/

 //for f in my_folder { cat $f; }