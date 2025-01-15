/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 22:33:29 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:26:33 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_command_node(Command *cmd, builtin_cmd_t *builtins,
		envvar **env_list, unset_path_flag *unset_flag)
{
	if (ft_execute_builtin(cmd, builtins, env_list, unset_flag) == 0)
		return (0);
	else
		return (ft_execute_external(cmd->args, cmd, env_list));
}

static void	error_mess(char *s)
{
	perror(s);
	exit(EXIT_FAILURE);
}

void	execute_single_command(Command *current_cmd, int *prev_pipe_fd,
		int *last_pid, builtin_cmd_t *builtins, envvar **env_list,
		unset_path_flag *unset_flag)
{
	int		current_pipe_fd[2];
	pid_t	pid;

	if (current_cmd->next && pipe(current_pipe_fd) == -1)
		error_mess("minishell: pipe");
	signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		handle_child_process(current_cmd, *prev_pipe_fd, current_pipe_fd,
			builtins, env_list, unset_flag);
	}
	else if (pid > 0)
	{
		handle_parent_process(pid, prev_pipe_fd, current_pipe_fd, current_cmd,
			last_pid);
		signal(SIGINT, handle_signal);
	}
	else
		error_mess("minishell: fork");
}

void	execute_pipeline(Command *cmd, builtin_cmd_t *builtins,
		envvar **env_list, shell_status *e_status, unset_path_flag *unset_flag)
{
	int		prev_pipe_fd;
	int		last_pid;
	Command	*current_cmd;

	prev_pipe_fd = STDIN_FILENO;
	last_pid = -1;
	current_cmd = cmd;
	execute_external_running = 1;
	while (current_cmd)
	{
		if (current_cmd->heredoc_delim)
			handle_heredoc_to_fd(current_cmd);
		execute_single_command(current_cmd, &prev_pipe_fd, &last_pid, builtins,
			env_list, unset_flag);
		current_cmd = current_cmd->next;
	}
	wait_for_pipeline(last_pid, e_status);
	execute_external_running = 0;
}

int	ft_execute_parsed_commands(Command *cmd, builtin_cmd_t *builtins,
		envvar **env_list, shell_status *e_status, unset_path_flag *unset_flag)
{
	if (!cmd || !cmd->args)
		return (0);
	if (cmd->heredoc_delim && !cmd->next)
		handle_heredoc_to_fd(cmd);
	if (!cmd->next)
		return (execute_command_node(cmd, builtins, env_list, unset_flag));
	else
	{
		execute_pipeline(cmd, builtins, env_list, e_status, unset_flag);
		return (0);
	}
}
