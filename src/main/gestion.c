#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include "../../utils/cd.h"
#include "../../utils/pwd.h"
#include "../../utils/exit.h"
#include "../../utils/redirection.h"
#include "../../utils/prompt.h"
#include <stdbool.h>
#include "../../utils/gestion.h"
#include "../../utils/extern.h"
#include "../../utils/for.h"

#define ARG_MAX 512

// TODO : documentation
void gestion_cmd(char *input, char **arg, char **cmd)
{
    char *espace = strchr(input, ' ');
    if(input[0]=='\0'){
        sprintf(*cmd ,"%c", '\0');
    }
    if (espace != NULL)
    {
        int indice_espace = espace - input;
        *arg = (input + indice_espace + 1);

    }
    if (espace != NULL && strlen(*arg) == 0)
    {
        snprintf(*cmd, strlen(input), "%s", input);
    }
    else if (espace == NULL)
    {
        snprintf(*cmd, strlen(input)+1, "%s", input);
    }
    else if (strlen(*arg) >= 1)
    {
        snprintf(*cmd, (strlen(input) - strlen(*arg) ), "%s", input);
    }
}