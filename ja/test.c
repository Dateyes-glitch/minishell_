// #include "minishell.h"


// void	handle_child_process(Command *current_cmd, int prev_pipe_fd,
// 		int current_pipe_fd[2], builtin_cmd_t *builtins, envvar **env_list,
// 		unset_path_flag *unset_flag)
// {
// 	if (prev_pipe_fd != STDIN_FILENO)
// 	{
// 		dup2(prev_pipe_fd, STDIN_FILENO);
// 		close(prev_pipe_fd);
// 	}
// 	if (current_cmd->next)
// 	{
// 		dup2(current_pipe_fd[1], STDOUT_FILENO);
// 		close(current_pipe_fd[1]);
// 	}
// 	if (current_cmd->next)
// 	{
// 		close(current_pipe_fd[0]);
// 	}
// 	execute_command_node(current_cmd, builtins, env_list, unset_flag);
// 	exit(EXIT_FAILURE);
// }

// void	handle_parent_process(pid_t pid, int *prev_pipe_fd,
// 		int current_pipe_fd[2], Command *current_cmd, int *last_pid)
// {
// 	*last_pid = pid;
// 	if (*prev_pipe_fd != STDIN_FILENO)
// 	{
// 		close(*prev_pipe_fd);
// 	}
// 	if (current_cmd->next)
// 	{
// 		close(current_pipe_fd[1]);
// 	}
// 	*prev_pipe_fd = current_pipe_fd[0];
// }

// void	wait_for_pipeline(int last_pid, shell_status *e_status)
// {
// 	int		exit_status;
// 	pid_t	pid;

// 	pid = waitpid(-1, &exit_status, 0);
// 	while (pid > 0)
// 	{
// 		if (pid == last_pid)
// 		{
// 			if (WIFEXITED(exit_status))
// 			{
// 				e_status->last_exit_status = WEXITSTATUS(exit_status);
// 			}
// 			else if (WIFSIGNALED(exit_status))
// 			{
// 				e_status->last_exit_status = 128 + WTERMSIG(exit_status);
// 			}
// 		}
// 		pid = waitpid(-1, &exit_status, 0);
// 	}
// }

// int	execute_command_node(Command *cmd, builtin_cmd_t *builtins,
// 		envvar **env_list, unset_path_flag *unset_flag)
// {
// 	if (ft_execute_builtin(cmd, builtins, env_list, unset_flag) == 0)
// 		return (0);
// 	else
// 		return (ft_execute_external(cmd->args, cmd, env_list));
// }


// void	execute_single_command(Command *current_cmd, int *prev_pipe_fd,
// 		int *last_pid, builtin_cmd_t *builtins, envvar **env_list,
// 		unset_path_flag *unset_flag)
// {
// 	int		current_pipe_fd[2];
// 	pid_t	pid;

// 	if (current_cmd->next && pipe(current_pipe_fd) == -1)
// 		error_mess("minishell: pipe");
// 	signal(SIGINT, SIG_IGN);
// 	pid = fork();
// 	if (pid == 0)
// 	{
// 		signal(SIGINT, SIG_DFL);
// 		handle_child_process(current_cmd, *prev_pipe_fd, current_pipe_fd,
// 			builtins, env_list, unset_flag);
// 	}
// 	else if (pid > 0)
// 	{
// 		handle_parent_process(pid, prev_pipe_fd, current_pipe_fd, current_cmd,
// 			last_pid);
// 		signal(SIGINT, handle_signal);
// 	}
// 	else
// 		error_mess("minishell: fork");
// }

// void	execute_pipeline(Command *cmd, builtin_cmd_t *builtins,
// 		envvar **env_list, shell_status *e_status, unset_path_flag *unset_flag)
// {
// 	int		prev_pipe_fd;
// 	int		last_pid;
// 	Command	*current_cmd;

// 	prev_pipe_fd = STDIN_FILENO;
// 	last_pid = -1;
// 	current_cmd = cmd;
// 	execute_external_running = 1;
// 	while (current_cmd)
// 	{
// 		if (current_cmd->heredoc_delim)
// 			handle_heredoc_to_fd(current_cmd);
// 		execute_single_command(current_cmd, &prev_pipe_fd, &last_pid, builtins,
// 			env_list, unset_flag);
// 		current_cmd = current_cmd->next;
// 	}
// 	wait_for_pipeline(last_pid, e_status);
// 	execute_external_running = 0;
// }

// int	ft_execute_parsed_commands(Command *cmd, builtin_cmd_t *builtins,
// 		envvar **env_list, shell_status *e_status, unset_path_flag *unset_flag)
// {
// 	if (!cmd || !cmd->args)
// 		return (0);
// 	if (cmd->heredoc_delim && !cmd->next)
// 		handle_heredoc_to_fd(cmd);
// 	if (!cmd->next)
// 		return (execute_command_node(cmd, builtins, env_list, unset_flag));
// 	else
// 	{
// 		execute_pipeline(cmd, builtins, env_list, e_status, unset_flag);
// 		return (0);
// 	}
// }

