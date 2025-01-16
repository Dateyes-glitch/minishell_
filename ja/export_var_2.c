/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_var_2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 22:26:18 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:28:39 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	if (value)
		new_var->value = strdup(value);
	else
		new_var->value = NULL;
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
