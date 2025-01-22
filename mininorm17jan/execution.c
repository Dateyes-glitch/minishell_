/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kforfoli <kforfoli@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 22:33:29 by hawild            #+#    #+#             */
/*   Updated: 2025/01/22 21:12:36 by kforfoli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_command_node(t_command *cmd, t_builtin_cmd *builtins,
		t_envvar **env_list)
{
	if (!cmd->args && cmd->heredoc_delim != NULL)
		return (0);
	if (strcmp(cmd->args[0], "exit") == 0)
		return(ft_execute_builtin(cmd, builtins, env_list));
	if (ft_execute_builtin(cmd, builtins, env_list) == 0)
		return (0);
	else
		return (ft_execute_external(cmd->args, cmd, env_list));
}

static void	error_mess(char *s)
{
	perror(s);
	exit(EXIT_FAILURE);
}

void	execute_single_command(t_command *current_cmd, t_pipeline_state *ps,
		t_builtin_cmd *builtins, t_envvar **env_list)
{
	pid_t	pid;

	if (current_cmd->next && pipe(ps->current_pipe_fd) == -1)
		error_mess("minishell: pipe");
	signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		handle_child_process(current_cmd, ps, builtins, env_list);
	}
	else if (pid > 0)
	{
		handle_parent_process(pid, current_cmd, ps);
		signal(SIGINT, handle_signal);
	}
	else
		error_mess("minishell: fork");
}

void	execute_pipeline(t_command *cmd, t_builtin_cmd *builtins,
		t_envvar **env_list, t_shell_status *e_status)
{
	t_pipeline_state	ps;
	t_command			*current_cmd;

	ps.prev_pipe_fd = STDIN_FILENO;
	ps.last_pid = -1;
	current_cmd = cmd;
	g_execute_external_running = 1;
	while (current_cmd)
	{
		if (current_cmd->heredoc_delim)
			handle_heredoc_to_fd(current_cmd);
		execute_single_command(current_cmd, &ps, builtins, env_list);
		current_cmd = current_cmd->next;
	}
	wait_for_pipeline(ps.last_pid, e_status);
	// g_execute_external_running = 0;
}

int	ft_execute_parsed_commands(t_command *cmd, t_builtin_cmd *builtins,
		t_envvar **env_list, t_shell_status *e_status)
{
	if (!cmd ||(!cmd->args && cmd->heredoc_delim == NULL))
		return (0);
	if (cmd->heredoc_delim && !cmd->next)
		handle_heredoc_to_fd(cmd);
	if (!cmd->next)
		return (execute_command_node(cmd, builtins, env_list));
	else
	{
		execute_pipeline(cmd, builtins, env_list, e_status);
		return (0);
	}
}
