/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external_execution.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 20:46:10 by hawild            #+#    #+#             */
/*   Updated: 2025/01/22 17:50:17 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_executable_path(char **args, t_command *cmd, t_envvar **env_list)
{
	char	*executable_path;
	char	*dup;
	char	*save;

	if (cmd->args[0][0] == '.' && cmd->args[0][1] == '/')
		executable_path = ft_strdup(cmd->args[0]);
	if (ft_strrchr(cmd->args[0], '/') != NULL)
	{
		save = ft_strrchr(cmd->args[0], '/');
		dup = ft_strdup(save);
		executable_path = find_executable(dup, env_list);
		free(dup);
	}
	else
		executable_path = find_executable(args[0], env_list);
	return (executable_path);
}

void	handle_command_not_found(t_command *cmd, char **args)
{
	if (cmd->env_exp != NULL)
	{
		printf("%s : is a directory\n", cmd->env_exp);
		cmd->exit_status = 126;
	}
	else
	{
		write(2, args[0], ft_strlen(args[0]));
		write(STDERR_FILENO, ": command not found", 19);
		write(STDERR_FILENO, "\n", 1);
		cmd->exit_status = 127;
	}
}

static void	handle_parent_processs(pid_t pid, t_command *cmd)
{
	int	status;

	if (waitpid(pid, &status, 0) == -1)
	{
		perror("waitpid");
		cmd->exit_status = 1;
	}
	else
	{
		if (WIFEXITED(status))
			cmd->exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			cmd->exit_status = 128 + WTERMSIG(status);
		else
			cmd->exit_status = 1;
	}
}

int	execute_fork(char *executable_path, char **args, char **envp,
		t_command *cmd)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		handle_redirections(cmd);
		execve(executable_path, args, envp);
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else if (pid > 0)
	{
		handle_parent_processs(pid, cmd);
		return (0);
	}
	else
	{
		perror("minishell: fork");
		cmd->exit_status = 1;
		return (-1);
	}
}

int	ft_execute_external(char **args, t_command *cmd, t_envvar **env_list)
{
	char	*executable_path;
	char	**envp;
	int		result;

	executable_path = get_executable_path(args, cmd, env_list);
	if (!executable_path)
	{
		handle_command_not_found(cmd, args);
		return (0);
	}
	envp = get_envp(env_list);
	if (!envp)
	{
		cmd->exit_status = 1;
		free(executable_path);
		return (1);
	}
	g_execute_external_running = 1;
	result = execute_fork(executable_path, args, envp, cmd);
	g_execute_external_running = 0;
	free(executable_path);
	return (result);
}
