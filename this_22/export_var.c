/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_var.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 22:28:16 by hawild            #+#    #+#             */
/*   Updated: 2025/01/22 17:46:01 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	display_export_vars(t_envvar **env_list)
{
	t_envvar	*current;

	current = *env_list;
	while (current)
	{
		printf("declare -x %s%c%s\n", current->key, '=', current->value);
		current = current->next;
	}
}

static void	update_env_var(t_envvar **env_list, char *key, char *value)
{
	t_envvar	*current;

	current = *env_list;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			free(current->value);
			current->value = ft_strdup(value);
			return ;
		}
		current = current->next;
	}
}

void	add_or_update_var(t_envvar **env_list, char *key, char *value)
{
	t_envvar	*current;

	current = *env_list;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			update_env_var(env_list, key, value);
			return ;
		}
		current = current->next;
	}
	add_env_var(env_list, key, value);
	return ;
}
