#include "minishell.h"

void	display_export_vars(envvar **env_list)
{
	envvar	*current;

	current = *env_list;
	while (current)
	{
		printf("declare -x %s%c%s\n", current->key, '=', current->value);
		current = current->next;
	}
}
static int	validate_env_key(char *key, unset_path_flag *unset_flag)
{
	if (key[0] >= '0' && key[0] <= '9')
	{
		printf("export: not an identifier: %s\n", key);
		return (0);
	}
	if (strcmp(key, "PATH") == 0)
		unset_flag->flag = 0;
	return (1);
}

static envvar	*create_new_env_var(char *key, char *value)
{
	envvar	*new_var;

	new_var = malloc(sizeof(envvar));
	if (!new_var)
	{
		perror("Failed to allocate memory for envvar");
		return (NULL);
	}
	new_var->key = strdup(key);
	new_var->value = value ? strdup(value) : NULL;
	new_var->next = NULL;
	return (new_var);
}

static void	append_env_var(envvar **env_list, envvar *new_var)
{
	envvar	*current;

	current = *env_list;
	if (!current)
	{
		*env_list = new_var;
		return ;
	}
	while (current->next != NULL)
		current = current->next;
	current->next = new_var;
}

void	add_env_var(envvar **env_list, char *key, char *value,
		unset_path_flag *unset_flag)
{
	envvar	*new_var;

	if (!validate_env_key(key, unset_flag))
		return ;
	new_var = create_new_env_var(key, value);
	if (!new_var)
		return ;
	append_env_var(env_list, new_var);
}

static void	update_env_var(envvar **env_list, char *key, char *value)
{
	envvar	*current;

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

void	add_or_update_var(envvar **env_list, char *key, char *value,
		unset_path_flag *unset_flag)
{
	envvar	*current;

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
	add_env_var(env_list, key, value, unset_flag);
	return ;
}
