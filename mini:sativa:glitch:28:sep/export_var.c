
#include "minishell.h"

void    display_export_vars(envvar **env_list)
{
    envvar *current;

    current = *env_list;
    while (current)
    {
        printf("declare -x %s%c%s\n", current->key, '=', current->value);
        current = current->next;
    }
}

static void    add_env_var(envvar  **env_list, char *key, char *value)
{
    envvar  *current;
    envvar  *previous;
    envvar  *new_var;

    current = *env_list;
    previous = NULL;
    while (current != NULL)
    {
        previous = current;
        current = current->next;
    }
    new_var = malloc(sizeof(envvar));
    if (!new_var)
    {
        perror("Failed to allocate memory for list");
        return ;
    }
    new_var->key = strdup(key);
    new_var->value = strdup(value);
    new_var->next = NULL;
    if (previous)
        previous->next = new_var;
    else
        *env_list = new_var;
}

static void    update_env_var(envvar **env_list, char *key, char *value)
{
    envvar *current;

    current = *env_list;
    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            free(current->value);
            current->value = strdup(value);
            return ;
        }
        current = current->next;
    }
}

void    add_or_update_var(envvar **env_list, char *key, char *value)
{
    envvar *current;
    
    current = *env_list;
    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            update_env_var(env_list, key, value);
            return ;
        }
        current = current->next;
    }
    add_env_var(env_list, key, value);
    return ;
}