/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_var_2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 22:26:18 by hawild            #+#    #+#             */
/*   Updated: 2025/01/17 18:55:09 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	validate_env_key(char *key)
{
	if (key[0] >= '0' && key[0] <= '9')
	{
		printf("export: not an identifier: %s\n", key);
		return (0);
	}
	return (1);
}

static t_envvar	*create_new_env_var(char *key, char *value)
{
	t_envvar	*new_var;

	new_var = malloc(sizeof(t_envvar));
	if (!new_var)
	{
		perror("Failed to allocate memory for envvar");
		return (NULL);
	}
	new_var->key = ft_strdup(key);
	if (value)
		new_var->value = ft_strdup(value);
	else
		new_var->value = NULL;
	new_var->next = NULL;
	return (new_var);
}

static void	append_env_var(t_envvar **env_list, t_envvar *new_var)
{
	t_envvar	*current;

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

void	add_env_var(t_envvar **env_list, char *key, char *value)
{
	t_envvar	*new_var;

	if (!validate_env_key(key))
		return ;
	new_var = create_new_env_var(key, value);
	if (!new_var)
		return ;
	append_env_var(env_list, new_var);
}
