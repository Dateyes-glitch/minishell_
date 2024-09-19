#include "minishell.h"
#include "env_var.h"

list *init_env(void)
{
    extern char **environ;
    size_t count, i;

    count = 0;
    i = 0;

    while(environ[count] != NULL)
        count++;
    list *env_cpy = malloc(sizeof(list));
    if (!env_cpy)
    {
        perror("Failed to allocate memory for list");
        return NULL;
    }
    env_cpy->var = malloc(count * sizeof(node));
    if (!env_cpy->var)
    {
        perror("Failed to allocate memory for node of list");
        free(env_cpy);
        return NULL;
    }

    env_cpy->count = count;
    while (i < count)
    {
        char *entry = environ[i];
        char *delim = strchr(entry, '=');

        if (!delim)
            continue;
        int key_length = delim - entry;
        int value_length = strlen(delim) + 1;

        env_cpy->var[i].key = malloc(key_length * sizeof(char *));
        if(!env_cpy->var[i].key)
        {
            perror("Memory allocation for name of environment variable failed");
            return NULL;
        }
        strncpy(env_cpy->var[i].key, entry, key_length);
        env_cpy->var[i].key[key_length] = '\0';
        env_cpy->var[i].value = malloc(value_length + 1 * sizeof(char *));
        if (!env_cpy->var[i].value)
        {
            perror("Memory allocation for value of environment variable failed");
            continue;
        }
        strcpy(env_cpy->var[i].value, delim + 1);
        i++;
    }
    return env_cpy;
        
}

