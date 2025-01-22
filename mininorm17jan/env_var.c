/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_var.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 22:25:29 by hawild            #+#    #+#             */
/*   Updated: 2025/01/17 18:55:32 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	display_env_vars(t_envvar **env_list)
{
	t_envvar	*current;
	int		flag;

	flag = 1;
	current = *env_list;
	while (current)
	{
		if (strcmp(current->key, "PATH") == 0)
			flag = 0;
		current = current->next;
	}
	current = *env_list;
	while (current && flag == 0)
	{
		printf("%s%c%s\n", current->key, '=', current->value);
		current = current->next;
	}
	if (flag == 1)
	{
		write(STDERR_FILENO, "command not found: env\n", 24);
		return ;
	}
}

void	change_shell_lvl(t_envvar **env_list)
{
	t_envvar	*current;
	char	*lvl;

	current = *env_list;
	lvl = "SHLVL";
	while (current)
	{
		if (current->key && strcmp(current->key, lvl) == 0)
		{
			free(current->value);
			current->value = ft_strdup("2");
		}
		current = current->next;
	}
	return ;
}

char	*get_env_var(t_envvar **env_list, char *key)
{
	t_envvar	*current;

	current = *env_list;
	while (current)
	{
		if (strcmp(current->key, key) == 0)
			return (current->value);
		current = current->next;
	}
	return (NULL);
}

void	unset_env_var(t_envvar **env_list, char *key)
{
	t_envvar	*current;
	t_envvar	*previous;

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
