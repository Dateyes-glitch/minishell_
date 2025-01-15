/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_initialization.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 21:04:06 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:22:10 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	new_node->key = strdup(key);
	new_node->value = strdup(value);
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
