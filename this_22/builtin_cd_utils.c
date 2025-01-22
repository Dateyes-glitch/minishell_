/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 20:05:16 by hawild            #+#    #+#             */
/*   Updated: 2025/01/22 17:43:56 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_envvar	*find_env_var(t_envvar *env_list, const char *key)
{
	t_envvar	*current;

	current = env_list;
	while (current != NULL)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			return (current);
		}
		current = current->next;
	}
	return (NULL);
}

void	update_oldpwd(t_envvar **env_list, const char *cwd)
{
	t_envvar	*var;

	var = find_env_var(*env_list, "OLDPWD");
	if (var)
	{
		free(var->value);
		var->value = ft_strdup(cwd);
	}
	else
		add_env_var(env_list, "OLDPWD", (char *)cwd);
}

void	update_pwd(t_envvar **env_list, const char *cwd)
{
	t_envvar	*var;

	var = find_env_var(*env_list, "PWD");
	if (var)
	{
		free(var->value);
		var->value = ft_strdup(cwd);
	}
	else
		add_env_var(env_list, "PWD", (char *)cwd);
}

int	handle_cd_home(t_command *cmd, t_envvar **env_list)
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

int	handle_cd_too_many_args(t_command *cmd)
{
	write(STDERR_FILENO, "minishell: cd: too many arguments\n", 35);
	cmd->exit_status = 1;
	return (1);
}
