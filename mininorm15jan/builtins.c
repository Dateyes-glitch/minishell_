/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 19:56:46 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:18:41 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	cmd_cd(Command *cmd, envvar **env_list, unset_path_flag *unset_flag)
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
	update_oldpwd(env_list, cwd, unset_flag);
	if (chdir(cmd->args[1]) != 0)
		return (ft_error(cmd, "minishell: cd"));
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (ft_error(cmd, "minishell: cd: getcwd"));
	update_pwd(env_list, cwd, unset_flag);
	cmd->exit_status = 0;
	return (0);
}

int	cmd_echo(Command *cmd, envvar **env_list, unset_path_flag *unset_flag)
{
	int	i;
	int	fd;
	int	newline;

	i = 1;
	newline = handle_newline_option(cmd, &i);
	echo_input_file(cmd);
	fd = open_output_file(cmd);
	if (fd == -1)
		return (1);
	write_args_to_output(cmd, fd, i);
	handle_newline_write(newline, fd);
	if (fd != STDOUT_FILENO)
		close(fd);
	if (cmd->exit_status != 1)
		cmd->exit_status = 0;
	return (0);
}

int	cmd_pwd(Command *cmd, envvar **env_list, unset_path_flag *unset_flag)
{
	char	cwd[1024];

	cmd->exit_status = 0;
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		write(STDOUT_FILENO, cwd, strlen(cwd));
		write(STDOUT_FILENO, "\n", 1);
	}
	else
		perror("getcwd");
	return (1);
}

int	cmd_unset(Command *cmd, envvar **env_list, unset_path_flag *unset_flag)
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
		unset_env_var(env_list, cmd->args[i], unset_flag);
		i++;
	}
	cmd->exit_status = 0;
	return (0);
}

int	cmd_env(Command *cmd, envvar **env_list, unset_path_flag *unset_flag)
{
	if (cmd->args[1] != NULL)
	{
		write(STDERR_FILENO, "minishell: env: too many arguments\n", 35);
		cmd->exit_status = 1;
		return (1);
	}
	display_env_vars(env_list, unset_flag);
	cmd->exit_status = 0;
	return (0);
}

int	cmd_exit(Command *cmd, envvar **env_list, unset_path_flag *unset_flag)
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
	exit_code = strtol(cmd->args[1], &endptr, 10);
	if (*endptr != '\0' || exit_code > INT_MAX || exit_code < INT_MIN)
	{
		write(STDERR_FILENO, "minishell: exit: ", 17);
		write(STDERR_FILENO, cmd->args[1], strlen(cmd->args[1]));
		write(STDERR_FILENO, ": numeric argument required\n", 28);
		exit(2);
	}
	exit((int)exit_code);
}
