
#include "env_var.h"

void    print_list(envvar *env_list)
{
    while (env_list)
    {
        printf("%s%c%s\n", env_list->key, '=', env_list->value);
        env_list = env_list->next;
    }
}

void    add_to_list(envvar **env_list, char *key, char *value)
{
    envvar  *new_node;
    envvar  *current;

    new_node = malloc(sizeof(envvar));
    if (!new_node)
    {
        perror("Failed to allocate memory for list");
        return ;
    }
    new_node->key = strdup(key);
    new_node->value = strdup(value);
    new_node->export_flag = 0;
    new_node->next = NULL;
    
    if (*env_list == NULL)
        *env_list = new_node;
    else
    {
        current = *env_list;
        while (current->next != NULL)
            current = current->next;
        current->next = new_node;
    }
}

void    initialize_env(envvar **env_list)
{
    extern char **environ;
    char **env;
    char *name;
    char *value;
    char    *split_pos;

    env = environ;
    while (*env)
    {
        split_pos = strchr(*env, '=');
        if (split_pos != NULL)
        {
            *split_pos = '\0';
            name = *env;
            value = split_pos + 1;
            add_to_list(env_list, name, value);
        }
        env++;
    }
}

char *get_env_var(envvar **env_list, char *key)
{
    envvar *current;

    current = *env_list;
    while (current)
    {
        if (strcmp(current->key, key) == 0)
            return (current->value);
        current = current->next;
    }
    return (NULL);
}

void    unset_env_var(envvar **env_list, char *key)
{
    envvar *current;
    envvar *previous;

    current = *env_list;
    previous = NULL;
    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            if (previous)
                previous->next = current->next;
            else
                *env_list = current->next;
            free(current->key);
            free(current->value);
            free(current);
            return ;
        }
        previous = current;
        current = current->next;
    }
}

void    ft_free(envvar **env_list)
{
    envvar  *current;
    envvar  *next_node;

    current = *env_list;
    while (current)
    {
        next_node = current->next;
        free(current->key);
        free(current->value);
        free(current->export_flag);
        free(current);
        current = next_node;
    }
    *env_list = NULL;
}

int main()
{
    envvar *env_list = NULL;
    char *var = "HELLO";

    initialize_env(&env_list);
    // print_list(env_list);
    add_or_update_var(&env_list, var, "123");
    // printf("\n\n\n");
    // print_list(env_list);
    add_or_update_var(&env_list, var, "123456");
    add_or_update_var(&env_list, "BLA", "bitch");
    add_or_update_var(&env_list, "YESSIR", "please");
    printf("\n\n\n");
    print_list(env_list);
    // display_export_vars(&env_list);
    // char *var2 = get_env_var(&env_list, "HELLO");
    // printf("\n\n\n");
    // printf("var : %s\n", var2);
    unset_env_var(&env_list, "BLA");
    printf("\n\n\n");
    print_list(env_list);
    return 0;
}