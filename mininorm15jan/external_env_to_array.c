/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external_env_to_array.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 20:41:00 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:30:33 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_env_vars(envvar **env_list)
{
	int		env_count;
	envvar	*current;

	env_count = 0;
	current = *env_list;
	while (current != NULL)
	{
		env_count++;
		current = current->next;
	}
	return (env_count);
}

char	**allocate_env_array(int env_count)
{
	char	**envp;

	envp = malloc((env_count + 1) * sizeof(char *));
	if (!envp)
	{
		perror("malloc");
		return (NULL);
	}
	return (envp);
}

int	populate_env_array(char **envp, envvar **env_list)
{
	envvar	*current;
	int		i;
	size_t	len;

	i = 0;
	current = *env_list;
	while (current != NULL)
	{
		len = strlen(current->key) + strlen(current->value) + 2;
		envp[i] = malloc(len);
		if (!envp[i])
		{
			perror("malloc");
			return (-1);
		}
		snprintf(envp[i], len, "%s=%s", current->key, current->value);
		i++;
		current = current->next;
	}
	return (0);
}

char	**convert_env_list_to_array(envvar **env_list)
{
	int		env_count;
	char	**envp;

	envp = allocate_env_array(env_count);
	env_count = count_env_vars(env_list);
	if (!envp)
		return (NULL);
	if (populate_env_array(envp, env_list) == -1)
	{
		free(envp);
		return (NULL);
	}
	envp[env_count] = NULL;
	return (envp);
}

char	**get_envp(envvar **env_list)
{
	char	**envp;

	envp = convert_env_list_to_array(env_list);
	if (!envp)
		return (NULL);
	return (envp);
}
