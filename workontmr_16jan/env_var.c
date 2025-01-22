#include "minishell.h"

void	display_env_vars(envvar **env_list, unset_path_flag *unset_flag)
{
	envvar	*current;

	current = *env_list;
	while (current && !unset_flag->flag)
	{
		printf("%s%c%s\n", current->key, '=', current->value);
		current = current->next;
	}
	if (unset_flag->flag == 1)
	{
		write(STDERR_FILENO, "command not found: env\n", 24);
		return ;
	}
}

void	change_shell_lvl(envvar **env_list)
{
	envvar	*current;
	char	*lvl;

	current = *env_list;
	lvl = "SHLVL";
	while (current)
	{
		if (current->key && strcmp(current->key, lvl) == 0)
			current->value = strdup("2");
		current = current->next;
	}
	return ;
}

void	add_to_list(envvar **env_list, char *key, char *value)
{
	envvar	*new_node;
	envvar	*current;

	new_node = malloc(sizeof(envvar));
	if (!new_node)
	{
		perror("Failed to allocate memory for list");
		return ;
	}
	new_node->key = ft_strdup(key);
	new_node->value = ft_strdup(value);
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

void	initialize_env(envvar **env_list)
{
	extern char	**environ;
	char		**env;
	char		*env_cpy;
	char		*name;
	char		*value;
	char		*split_pos;

	env = environ;
	while (*env)
	{
		env_cpy = strdup(*env);
		if (!env_cpy)
			return ;
		split_pos = strchr(env_cpy, '=');
		if (split_pos != NULL)
		{
			*split_pos = '\0';
			name = env_cpy;
			value = split_pos + 1;
			add_to_list(env_list, name, value);
		}
		free(env_cpy);
		env++;
	}
}

char	*get_env_var(envvar **env_list, char *key)
{
	envvar	*current;

	current = *env_list;
	while (current)
	{
		if (strcmp(current->key, key) == 0)
			return (current->value);
		current = current->next;
	}
	return (NULL);
}

void	unset_env_var(envvar **env_list, char *key, unset_path_flag *unset_flag)
{
	envvar	*current;
	envvar	*previous;

	current = *env_list;
	previous = NULL;
	while (current)
	{
		if (strcmp(current->key, key) == 0)
		{
			if (strcmp(key, "PATH") == 0)
				unset_flag->flag = 1;
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

void	ft_free(envvar **env_list)
{
	envvar	*current;
	envvar	*next_node;

	current = *env_list;
	while (current)
	{
		next_node = current->next;
		free(current->key);
		free(current->value);
		free(current);
		current = next_node;
	}
	*env_list = NULL;
}
