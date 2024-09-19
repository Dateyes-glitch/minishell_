#ifndef ENV_VAR
#define ENV_VAR

#include "minishell.h"

typedef struct env_var
{
    char *key;
    char *value;
} node;

typedef struct env_list
{
    node *var;
    size_t count;
} list;

list *init_env(void);


#endif