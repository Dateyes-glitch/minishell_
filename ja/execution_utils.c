/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 22:54:29 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:25:54 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	write_to_temp_file(int fd, const char *delim)
{
	char	*input;

	input = NULL;
	while (1)
	{
		input = readline("heredoc> ");
		if (input == NULL || strcmp(input, delim) == 0)
		{
			free(input);
			break ;
		}
		write(fd, input, strlen(input));
		write(fd, "\n", 1);
		free(input);
	}
}

void	handle_heredoc_to_fd(Command *cmd)
{
	char	temp_file[256];
	int		fd;

	if (!cmd || !cmd->heredoc_delim)
	{
		cmd->exit_status = 1;
		return ;
	}
	create_unique_temp_file(temp_file, &fd);
	if (fd == -1)
	{
		cmd->exit_status = 1;
		return ;
	}
	write_to_temp_file(fd, cmd->heredoc_delim);
	close(fd);
	cmd->input_file = strdup(temp_file);
}

void	handle_child_process(Command *current_cmd, int prev_pipe_fd,
		int current_pipe_fd[2], builtin_cmd_t *builtins, envvar **env_list,
		unset_path_flag *unset_flag)
{
	if (prev_pipe_fd != STDIN_FILENO)
	{
		dup2(prev_pipe_fd, STDIN_FILENO);
		close(prev_pipe_fd);
	}
	if (current_cmd->next)
	{
		dup2(current_pipe_fd[1], STDOUT_FILENO);
		close(current_pipe_fd[1]);
	}
	if (current_cmd->next)
	{
		close(current_pipe_fd[0]);
	}
	execute_command_node(current_cmd, builtins, env_list, unset_flag);
	exit(EXIT_FAILURE);
}

void	handle_parent_process(pid_t pid, int *prev_pipe_fd,
		int current_pipe_fd[2], Command *current_cmd, int *last_pid)
{
	*last_pid = pid;
	if (*prev_pipe_fd != STDIN_FILENO)
	{
		close(*prev_pipe_fd);
	}
	if (current_cmd->next)
	{
		close(current_pipe_fd[1]);
	}
	*prev_pipe_fd = current_pipe_fd[0];
}

void	wait_for_pipeline(int last_pid, shell_status *e_status)
{
	int		exit_status;
	pid_t	pid;

	pid = waitpid(-1, &exit_status, 0);
	while (pid > 0)
	{
		if (pid == last_pid)
		{
			if (WIFEXITED(exit_status))
			{
				e_status->last_exit_status = WEXITSTATUS(exit_status);
			}
			else if (WIFSIGNALED(exit_status))
			{
				e_status->last_exit_status = 128 + WTERMSIG(exit_status);
			}
		}
		pid = waitpid(-1, &exit_status, 0);
	}
}
