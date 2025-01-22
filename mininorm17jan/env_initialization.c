/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_initialization.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 21:04:06 by hawild            #+#    #+#             */
/*   Updated: 2025/01/17 18:55:32 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	add_to_list(t_envvar **env_list, char *key, char *value)
{
	t_envvar	*new_node;
	t_envvar	*current;

	new_node = malloc(sizeof(t_envvar));
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

void	initialize_env(t_envvar **env_list)
{
	extern char	**environ;
	char		**env;
	char		*env_cpy;
	char		*split_pos;

	env = environ;
	while (*env)
	{
		env_cpy = ft_strdup(*env);
		if (!env_cpy)
			return ;
		split_pos = strchr(env_cpy, '=');
		if (split_pos != NULL)
		{
			*split_pos = '\0';
			add_to_list(env_list, env_cpy, (split_pos + 1));
		}
		free(env_cpy);
		env++;
	}
}
