/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 19:56:46 by hawild            #+#    #+#             */
/*   Updated: 2025/01/22 20:24:05 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	cmd_cd(t_command *cmd, t_envvar **env_list)
{
	char	cwd[1024];

	if (cmd->args[1] == NULL)
	{
		if (handle_cd_home(cmd, env_list))
			return (1);
	}
	else if (cmd->args[2] != NULL)
		return (handle_cd_too_many_args(cmd));
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (ft_error(cmd, "minishell: cd: getcwd"));
	update_oldpwd(env_list, cwd);
	if (chdir(cmd->args[1]) != 0)
		return (ft_error(cmd, "minishell: cd"));
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (ft_error(cmd, "minishell: cd: getcwd"));
	update_pwd(env_list, cwd);
	cmd->exit_status = 0;
	return (0);
}

int	cmd_pwd(t_command *cmd, t_envvar **env_list)
{
	char	cwd[1024];

	cmd->exit_status = 0;
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		write(STDOUT_FILENO, cwd, ft_strlen(cwd));
		write(STDOUT_FILENO, "\n", 1);
	}
	else
		perror("getcwd");
	return (1);
}

int	cmd_unset(t_command *cmd, t_envvar **env_list)
{
	int	i;

	if (cmd->args[1] == NULL)
	{
		cmd->exit_status = 0;
		return (0);
	}
	i = 1;
	while (cmd->args[i] != NULL)
	{
		unset_env_var(env_list, cmd->args[i]);
		i++;
	}
	cmd->exit_status = 0;
	return (0);
}

int	cmd_env(t_command *cmd, t_envvar **env_list)
{
	if (cmd->args[1] != NULL)
	{
		write(STDERR_FILENO, "minishell: env: too many arguments\n", 35);
		cmd->exit_status = 1;
		return (1);
	}
	display_env_vars(env_list);
	cmd->exit_status = 0;
	return (0);
}

int	cmd_exit(t_command *cmd, t_envvar **env_list)
{
	char	*endptr;
	long	exit_code;

	if (cmd->args[1] == NULL)
		exit(0);
	if (cmd->args[2] != NULL)
	{
		write(STDERR_FILENO, "minishell: exit: too many arguments\n", 36);
		cmd->exit_status = 1;
		return (1);
	}
	exit_code = ft_strtol(cmd->args[1], &endptr, 10);
	if (*endptr != '\0' || exit_code > INT_MAX || exit_code < INT_MIN)
	{
		write(STDERR_FILENO, "minishell: exit: ", 17);
		write(STDERR_FILENO, cmd->args[1], ft_strlen(cmd->args[1]));
		write(STDERR_FILENO, ": numeric argument required\n", 28);
		exit(2);
	}
	exit((int)exit_code);
}
