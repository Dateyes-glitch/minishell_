/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_var.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 22:25:29 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 22:25:40 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
