/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 20:05:16 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:13:24 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

envvar	*find_env_var(envvar *env_list, const char *key)
{
	envvar	*current;

	current = env_list;
	while (current != NULL)
	{
		if (strcmp(current->key, key) == 0)
		{
			return (current);
		}
		current = current->next;
	}
	return (NULL);
}

void	update_oldpwd(envvar **env_list, const char *cwd,
		unset_path_flag *unset_flag)
{
	envvar	*var;

	var = find_env_var(*env_list, "OLDPWD");
	if (var)
	{
		free(var->value);
		var->value = strdup(cwd);
	}
	else
		add_env_var(env_list, "OLDPWD", (char *)cwd, unset_flag);
}

void	update_pwd(envvar **env_list, const char *cwd,
		unset_path_flag *unset_flag)
{
	envvar	*var;

	var = find_env_var(*env_list, "PWD");
	if (var)
	{
		free(var->value);
		var->value = strdup(cwd);
	}
	else
		add_env_var(env_list, "PWD", (char *)cwd, unset_flag);
}

int	handle_cd_home(Command *cmd, envvar **env_list)
{
	char	*home;

	home = getenv("HOME");
	if (home == NULL)
	{
		write(STDERR_FILENO, "minishell: cd: HOME not set\n", 28);
		cmd->exit_status = 1;
		return (1);
	}
	cmd->args[1] = home;
	return (0);
}

int	handle_cd_too_many_args(Command *cmd)
{
	write(STDERR_FILENO, "minishell: cd: too many arguments\n", 35);
	cmd->exit_status = 1;
	return (1);
}
