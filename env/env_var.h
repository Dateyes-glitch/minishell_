#ifndef ENV_VAR_H
# define ENV_VAR_H


//# include "minishell.h"
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

typedef struct envvar 
{
    char *key;
    char *value;
    int  export_flag;
    struct envvar *next;
}   envvar;

void    print_list(envvar *env_list);
void    add_to_list(envvar **env_list, char *key, char *value);
void    initialize_env(envvar **env_list);
void    add_or_update_var(envvar **env_list, char *key, char *value);
char    *get_env_var(envvar **env_list, char *key);
void    display_export_vars(envvar **env_list);
void    unset_env_var(envvar **env_list, char *key);
void    ft_free(envvar **env_list);

#endif