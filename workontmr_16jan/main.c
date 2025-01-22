#include "minishell.h"

void clear_buffer() {
    rl_replace_line("", 0); // Replace current line with an empty string
    rl_redisplay();        // Refresh the display
}


void	handle_signal(int sig)
{
	if (sig == SIGINT)
	{
		if (execute_external_running == 1)
		{
			printf("\n");
			clear_buffer();
		}
		else
		{
			printf("\n");
			rl_on_new_line();
			clear_buffer();
			// rl_replace_line("", 0);
			// rl_redisplay();
		}
		execute_external_running = 130;
	}
	else if (sig == SIGQUIT)
	{
		if(execute_external_running == 1)
		{
			printf("QUIT\n");
			clear_buffer();
			execute_external_running = 131;
		}
		else
		{
			rl_on_new_line();
			rl_redisplay();
		}
	}
}
int	check_permissions(char *filename)
{
	if (access(filename, F_OK) != 0)
		return 3;
	if (access(filename, R_OK) != 0)
		return 1;
	if (access(filename, W_OK) != 0)
		return 2;
	return 0;
}
static void	handle_output_redirection(Command *cmd, int append)
{
	int	fd_out;

	if (append)
		fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_out < 0)
	{
		write(2, "minishell: ", 11);
		write(2, cmd->output_file, strlen(cmd->output_file));
		write(2, ": file not found\n", 17);
		cmd->exit_status = 1;
		exit(EXIT_FAILURE);
	}
	if (dup2(fd_out, STDOUT_FILENO) < 0)
	{
		perror("minishell: dup2 output");
		close(fd_out);
		cmd->exit_status = 1;
		exit(EXIT_FAILURE);
	}
	close(fd_out);
}

static void	handle_input_redirection(Command *cmd)
{
	int	fd_in;

	fd_in = open(cmd->input_file, O_RDONLY);
	if (fd_in < 0)
	{
		write(2, "minishell: ", 11);
		write(2, cmd->input_file, strlen(cmd->input_file));
		write(2, ": input file not found\n", 23);
		cmd->exit_status = 1;
		exit(EXIT_FAILURE);
	}
	if (dup2(fd_in, STDIN_FILENO) < 0)
	{
		perror("minishell: dup2 input");
		close(fd_in);
		cmd->exit_status = 1;
		exit(EXIT_FAILURE);
	}
	close(fd_in);
}

static void	handle_redirections_for_output(Command *cmd)
{
	if (cmd->output_file)
		handle_output_redirection(cmd, cmd->append);
}

static void	handle_redirections_for_input(Command *cmd)
{
	if (cmd->input_file)
		handle_input_redirection(cmd);
}

void	handle_redirections(Command *cmd)
{
	if (!cmd || !cmd->args)
		return ;
	if (cmd->output == 1)
	{
		handle_redirections_for_output(cmd);
		handle_redirections_for_input(cmd);
	}
	else
	{
		handle_redirections_for_input(cmd);
		handle_redirections_for_output(cmd);
	}
}
void	create_unique_temp_file(char *temp_file, int *fd)
{
	int	counter;

	counter = 0;
	while (1)
	{
		sprintf(temp_file, "/tmp/heredoc_%d", counter);
		*fd = open(temp_file, O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0644);
		if (*fd != -1)
		{
			break ; // File successfully created
		}
		if (errno != EEXIST)
		{
			perror("minishell: creating temporary file for heredoc");
			return ;
		}
		counter++;
	}
}

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

int	execute_command_node(Command *cmd, builtin_cmd_t *builtins,
		envvar **env_list, unset_path_flag *unset_flag)
{
	if (!cmd->args && cmd->heredoc_delim != NULL)
		return (0);
	if (strcmp(cmd->args[0], "exit") == 0)
		return(ft_execute_builtin(cmd, builtins, env_list, unset_flag));
	if (ft_execute_builtin(cmd, builtins, env_list, unset_flag) == 0)
		return (0);
	else
		return (ft_execute_external(cmd->args, cmd, env_list));
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
		close(current_pipe_fd[0]); // Close read end of current pipe
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

	while ((pid = waitpid(-1, &exit_status, 0)) > 0)
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
	}
}

void	execute_pipeline(Command *cmd, builtin_cmd_t *builtins,
		envvar **env_list, shell_status *e_status, unset_path_flag *unset_flag)
{
	execute_external_running = 1;
	int		prev_pipe_fd;
	int		current_pipe_fd[2];
	pid_t	pid;
	int		last_pid;
	Command	*current_cmd;
	execute_external_running = 1;
	prev_pipe_fd = STDIN_FILENO;
	last_pid = -1;
	current_cmd = cmd;
	while (current_cmd)
	{
		if (current_cmd->heredoc_delim)
		{
			handle_heredoc_to_fd(current_cmd);
		}
		if (current_cmd->next && pipe(current_pipe_fd) == -1)
		{
			perror("minishell: pipe");
			exit(EXIT_FAILURE);
		}
		signal(SIGINT, SIG_IGN);
		if ((pid = fork()) == 0)
		{
			signal(SIGINT, SIG_DFL);
			handle_child_process(current_cmd, prev_pipe_fd, current_pipe_fd,
				builtins, env_list, unset_flag);
		}
		else if (pid > 0)
		{
			handle_parent_process(pid, &prev_pipe_fd, current_pipe_fd,
				current_cmd, &last_pid);
			signal(SIGINT, handle_signal);
		}
		else
		{
			perror("minishell: fork");
			exit(EXIT_FAILURE);
		}
		current_cmd = current_cmd->next;
	}
	wait_for_pipeline(last_pid, e_status);
	// execute_external_running = 0;
}

int	ft_execute_parsed_commands(Command *cmd, builtin_cmd_t *builtins,
		envvar **env_list, shell_status *e_status, unset_path_flag *unset_flag)
{
	if (!cmd || (!cmd->args && cmd->heredoc_delim == NULL))
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

size_t	ft_strlcpy(char *dest, const char *src, size_t n)
{
	size_t	i;
	size_t	len;

	i = 0;
	len = strlen(src);
	if (n == 0)
		return (len);
	if (n != 0)
	{
		while (src[i] != '\0' && i < (n - 1))
		{
			dest[i] = src[i];
			i++;
		}
		dest[i] = '\0';
	}
	return (len);
}

int	handle_exit_status(Command *to_cpy_cmd)
{
	int		i;
	Command	*cmd;

	i = 0;
	// int j = 0;
	cmd = to_cpy_cmd;
	while (cmd != NULL)
	{
		i++;
		cmd = cmd->next;
	}
	return (i - 1);
}

char	*ft_substr(const char *s, unsigned int start, size_t len)
{
	char			*ret;
	unsigned int	i;

	if (!s)
		return (0);
	i = strlen(s);
	if (i < start)
	{
		return (strdup(""));
	}
	else if (strlen(s + start) < len && len != 0)
		len = strlen(s + start);
	ret = malloc(sizeof(char) * (len + 1));
	if (!ret)
		return (0);
	ft_strlcpy(ret, s + start, len + 1);
	return (ret);
}

void ft_run_shell(void) 
{
    char *input = NULL;
    Token *tokens;
    Command *commands;
    int status = 0;
    builtin_cmd_t builtins[NUM_BUILTINS];
    envvar  *env_list = NULL;
    shell_status e_status;
    e_status.last_exit_status = 0;
    unset_path_flag unset_flag;
    unset_flag.flag = 0;
    initialize_env(&env_list);
    change_shell_lvl(&env_list);
    ft_init_builtins(builtins);

	 //	signal(SIGINT, handle_signal);
   // signal(SIGQUIT, handle_signal);

    do 
    {
			//continue;
        signal(SIGINT, handle_signal);
        signal(SIGQUIT, handle_signal);
        input = ft_read_input();
        if (!input)
		{
			ft_free(&env_list);
    // Clean up built-in command names
    		for (int i = 0; i < NUM_BUILTINS; i++)
        		free(builtins[i].name);
			exit(1);
		}
        tokens = tokenize_input(input);
        Token *another_t = tokens;
        if (tokens == NULL || tokens->type == TOKEN_ERROR)
        {
			if (tokens == NULL)
			{
			}
			else if (tokens->type == TOKEN_ERROR)
			{
				e_status.last_exit_status = 1;
				printf("minishell:unclosed quotes\n");
				free_tokens(tokens);
			}
			free(input);
			continue;
        }
		// e_status.last_exit_status = execute_external_running;
        commands = parse_pipeline (&tokens, &env_list, &e_status);
        if (!commands && e_status.last_exit_status == 0)
		{
			e_status.last_exit_status = 1;
			continue;

		}
		if (commands->exit_status == 2)
		{
			e_status.last_exit_status = 2;
			free_tokens(tokens);
			free_commands(commands);
			continue;
		}
		Command *point = commands;
		while (point && point->exit_status != 127 && point->exit_status != 300)
			point = point->next;
		if (point != NULL)
		{
			if (point->exit_status == 127)
				printf("minishell: permissions denied: %s\n", point->input_file);
			else if (point->exit_status == 300)
				printf("minishell: permissions denied: %s\n", point->output_file);
			e_status.last_exit_status = 1;
			free_tokens(tokens);
			free_commands(commands);
			continue;

		}
        status = ft_execute_parsed_commands(commands, builtins, &env_list, &e_status, &unset_flag);
		// printf("status: %d\n",status);
        Command *another = commands;
        int j = 0;
		while (another && another->next && (another->exit_status!=130 && another->exit_status != 131))
			another = another->next;
		e_status.last_exit_status = another->exit_status;
        free_tokens(tokens);
        free_commands(commands);
        free(input);
    	execute_external_running = 0;
    } while (status == 0);
    ft_free(&env_list);
    // Clean up built-in command names
    for (int i = 0; i < NUM_BUILTINS; i++)
        free(builtins[i].name);
	if (status == 400)
		exit(0);
	if (status == 500)
		exit(2);
	if (status != 0 && status != 1)
		exit(status);
	
}
