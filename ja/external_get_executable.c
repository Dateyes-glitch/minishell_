/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external_get_executable.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 20:21:13 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:37:00 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_path_variable(envvar **env_list)
{
	envvar	*current;

	current = *env_list;
	while (current != NULL)
	{
		if (strcmp(current->key, "PATH") == 0)
			return (current->value);
		current = current->next;
	}
	return (NULL);
}

char	*process_path_segment(char *dir_start, char *command)
{
	char	*full_path;

	full_path = malloc(strlen(dir_start) + strlen(command) + 2);
	if (!full_path)
		return (NULL);
	sprintf(full_path, "%s/%s", dir_start, command);
	if (access(full_path, X_OK) == 0)
		return (full_path);
	free(full_path);
	return (NULL);
}

char	*search_in_path(char *path_copy, char *command)
{
	char	*dir_start;
	char	*dir_end;
	char	*full_path;

	dir_start = path_copy;
	dir_end = strchr(dir_start, ':');
	while (dir_end != NULL)
	{
		*dir_end = '\0';
		full_path = process_path_segment(dir_start, command);
		if (full_path)
			return (full_path);
		dir_start = dir_end + 1;
		dir_end = strchr(dir_start, ':');
	}
	full_path = process_path_segment(dir_start, command);
	return (full_path);
}

char	*find_executable(char *command, envvar **env_list)
{
	char	*path;
	char	*path_copy;
	char	*full_path;

	path = get_path_variable(env_list);
	if (!path)
		return (NULL);
	path_copy = ft_strdup(path);
	if (!path_copy)
		return (NULL);
	full_path = search_in_path(path_copy, command);
	free(path_copy);
	return (full_path);
}
