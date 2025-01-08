#include "minishell.h"
#include "env_var.h"
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

// Signal handler for interactive mode
void handle_signal(int sig) 
{
    if (sig == SIGINT) 
    {
        // Ctrl-C: Display new prompt
        printf("\n");
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    } 
    else if (sig == SIGQUIT)
    {
        // Ctrl-\: Do nothing
    }
}

void check_permissions(char *filename) {
 
  if (access(filename, R_OK) != 0) {
    perror("Read permission"); 
  } 
  if (access(filename, W_OK) != 0) {
    perror("Write permission");
  } 
  if (access(filename, X_OK) != 0) {
    perror("Execute permission");
  } 
}
void handle_redirections(Command *cmd)
{
    int fd_in = 0;
    int fd_out = 1;

    if (!cmd || !cmd->args) return;

    if (cmd->output == 1)
    {
        if (cmd->output_file) 
        {
            if (cmd->append != 0)
                fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            else
                fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0)
            {
                write(2,"minishell: ",11);
                write(2,cmd->output_file, strlen(cmd->output_file));
                write(2,": file not found\n", 17);
                //perror("minishell: output file");
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
          
        if (cmd->input_file) 
        {
            fd_in = open(cmd->input_file, O_RDONLY);
            if (fd_in < 0) 
            {
                write(2,"minishell: ", 11);
                write(2,cmd->input_file, strlen(cmd->input_file));
                write(2,": input file not found\n",23);
                //perror("minishell: input file");
                cmd->exit_status = 1;
                exit(EXIT_FAILURE);
            }
            if (dup2(fd_in, STDIN_FILENO) < 0) 
            {
                perror("minishell: dup2 input");
                cmd->exit_status = 1;
                close(fd_in);
                exit(EXIT_FAILURE);
            }
            close(fd_in);
        }
    }
    else
    {

        if (cmd->input_file) 
        {
            fd_in = open(cmd->input_file, O_RDONLY);
            if (fd_in < 0) 
            {
                write(2,"minishell: ", 11);
                write(2,cmd->input_file, strlen(cmd->input_file));
                write(2,": input file not found\n",23);
                //perror("minishell: input file");
                cmd->exit_status = 1;
                exit(EXIT_FAILURE);
            }
            if (dup2(fd_in, STDIN_FILENO) < 0) 
            {
                perror("minishell: dup2 input");
                cmd->exit_status = 1;
                close(fd_in);
                exit(EXIT_FAILURE);
            }
            close(fd_in);
        }

        if (cmd->output_file) 
        {
            if (cmd->append != 0)
                fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            else
                fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0)
            {
                write(2,"minishell: ",11);
                write(2,cmd->output_file, strlen(cmd->output_file));
                write(2,": file not found\n", 17);
                //perror("minishell: output file");
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
        
    }
}


void handle_heredoc_to_fd(Command *cmd) 
{
    char *input = NULL;
    char temp_file[256]; // Adjust size if needed

    // Generate a unique filename using a loop and a counter
    int counter = 0;
    int fd;
    while (1) 
    {
        // Manually construct the filename string
        strcpy(temp_file, "/tmp/heredoc_");
        char counter_str[16]; // Adjust size if needed
        int i = 0;
        int temp = counter;
        if (temp == 0) 
        {
            counter_str[i++] = '0';
        } 
        else 
        {
            while (temp > 0) 
            {
                counter_str[i++] = (temp % 10) + '0';
                temp /= 10;
            }
            // Reverse the counter string
            for (int j = 0; j < i / 2; j++) 
            {
                char temp_char = counter_str[j];
                counter_str[j] = counter_str[i - j - 1];
                counter_str[i - j - 1] = temp_char;
            }
        }
        counter_str[i] = '\0';
        strcat(temp_file, counter_str);

        fd = open(temp_file, O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0644);
        if (fd != -1) break; // Success
        if (errno != EEXIST) 
        {
            perror("minishell: creating temporary file for heredoc");
            cmd->exit_status = 1;
            return;
        }
        counter++;
    }
    while (1)
    {
        input = readline("heredoc> ");

        if (input == NULL || strcmp(input, cmd->heredoc_delim) == 0) 
        {
            free(input); 
            break;
        }

        write(fd, input, strlen(input));
        write(fd, "\n", 1);
        free(input);
    }

    close(fd); 

    cmd->input_file = strdup(temp_file); 
    
}


int execute_command_node(Command *cmd, builtin_cmd_t *builtins, envvar **env_list, unset_path_flag *unset_flag) 
{
    if (ft_execute_builtin(cmd, builtins, env_list, unset_flag) == 0)
        return 0;
    else
        return ft_execute_external(cmd->args, cmd);
}


void execute_pipeline(Command *cmd, builtin_cmd_t *builtins, envvar **env_list, shell_status *e_status, unset_path_flag *unset_flag)
{
    int prev_pipe_fd = STDIN_FILENO;
    int current_pipe_fd[2];
    pid_t pid;
    Command *current_cmd = cmd;
    int exit_status;
    int last_pid = -1;

    while (current_cmd)
    {
        // 1. Handle heredoc before creating pipes
        if (current_cmd->heredoc_delim)
        {
            handle_heredoc_to_fd(current_cmd);
        }
        if (current_cmd->next)
        {
            if (pipe(current_pipe_fd) == -1)
            {
                perror("minishell: pipe");
                exit(EXIT_FAILURE);
            }
        }

        if ((pid = fork()) == 0)
        {
            // Child process
            if (cmd->heredoc_delim)
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

                execute_command_node(current_cmd, builtins, env_list, unset_flag);
                exit(EXIT_FAILURE);
            }
            else
            {
                if (prev_pipe_fd != STDIN_FILENO)
                dup2(prev_pipe_fd, STDIN_FILENO);

                if (current_cmd->next)
                    dup2(current_pipe_fd[1], STDOUT_FILENO);
            // Close all unnecessary pipe ends
                if (current_cmd->next) 
                    close(current_pipe_fd[0]);  // Close read end of current pipe
                if (prev_pipe_fd != STDIN_FILENO)
                    close(prev_pipe_fd);

                execute_command_node(current_cmd, builtins, env_list, unset_flag);
                exit(EXIT_FAILURE);

            }
        }
        else if (pid > 0)
        {
            // Parent process
            last_pid = pid;
            if (prev_pipe_fd != STDIN_FILENO)
                close(prev_pipe_fd);
            if (current_cmd->next)
                close(current_pipe_fd[1]);

            prev_pipe_fd = current_pipe_fd[0];
        }
        else
        {
            perror("minishell: fork");
            exit(EXIT_FAILURE);
        }

        current_cmd = current_cmd->next;
    }
     // Wait for all processes in the pipeline
    while (waitpid(-1, &exit_status, 0) > 0) {
    if (pid == last_pid) {  // Only track the last command's status
        if (WIFEXITED(exit_status)) {
            e_status->last_exit_status = WEXITSTATUS(exit_status);
        } else if (WIFSIGNALED(exit_status)) {
            e_status->last_exit_status = 128 + WTERMSIG(exit_status);
        }
    }
}
}


int ft_execute_parsed_commands(Command *cmd, builtin_cmd_t *builtins, envvar **env_list, shell_status *e_status, unset_path_flag *unset_flag) 
{
    if (!cmd || !cmd->args) return 0;
    if (cmd->heredoc_delim && !cmd->next)
        handle_heredoc_to_fd(cmd);
    if (!cmd->next) 
        return execute_command_node(cmd, builtins, env_list, unset_flag); 
    else 
    {
        execute_pipeline(cmd, builtins, env_list, e_status, unset_flag);
        return 0;
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

int     handle_exit_status(Command *to_cpy_cmd)
{
    int i = 0;
    //int j = 0;
    Command *cmd = to_cpy_cmd;
    while (cmd != NULL)
    {
        i++;
        cmd = cmd->next;
    }
    return (i-1);
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

void    expand_variables(Command **cmds, envvar **env_list)
{
    Command *current;
    int i;
    int j;
    int length;
    char *buffer;
    int flag;
    int g;

    current = *cmds;
    flag = 0;
    while (current)
    {
        i = 0;
        while (current->args[i])
        {
            length = strlen(current->args[i]);
            buffer = (char *)malloc(sizeof(char) * (length * 2));
            if (!buffer)
                return ;
            j = 0;
            g = 0;
            while (current->args[i][j])
            {
                if (current->args[i][j] == '$' && current->args[i][j + 1] != ' ' && current->args[i][j + 1] != '\t')
                {
                    j++;
                    size_t key_len = 0;
                    unsigned int start = j;
                    while (current->args[i][j] != ' ' && current->args[i][j] != '\t' && current->args[i][j] != '\0')
                    {    
                        key_len++;
                        j++;    
                    }
                    char *substr =  ft_substr(current->args[i], start, key_len);
                    char *value = get_env_var(env_list, substr);
                    int k = 0;
                    int val_len = strlen(value);
                    while (val_len > 0)
                    {
                        buffer[g++] = value[k++];
                        val_len--;
                    }
                    free(substr);
                    flag = 1;
                }
                else
                    buffer[g++] = current->args[i][j++];
            }
            buffer[g] = '\0';
            if (flag == 0)
                free(buffer);
            else if (flag == 1)
            {
                current->args[i] = strdup(buffer);
                free(buffer);
                flag = 0;
            }
            i++;
        }
        current = current->next;
    }
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
    //printf("%i\n", e_status.last_exit_status);
    unset_path_flag unset_flag;

    unset_flag.flag = 0;
    initialize_env(&env_list);
    change_shell_lvl(&env_list);
    ft_init_builtins(builtins);

    signal(SIGINT, handle_signal);
    signal(SIGQUIT, handle_signal);

    do 
    {
        
        // char cwd[1024];
        // if (getcwd(cwd, sizeof(cwd)) != NULL)
        //     printf("%s%s%s %s%s%s : ", BOLD_GREEN, ARROW, RESET, UNDERLINE_B_I_MAGENTA, cwd, RESET);
        // else
        //     perror("getcwd");

        input = ft_read_input();
        if (!input) break;
        tokens = tokenize_input(input);
        if (tokens == NULL || tokens->type == TOKEN_ERROR)
        {
            free(input);
            continue;
        }
        commands = parse_pipeline (&tokens, &env_list, &e_status);
        if (!commands)
            e_status.last_exit_status = 1;
        //expand_variables(&commands, &env_list);
        status = ft_execute_parsed_commands(commands, builtins, &env_list, &e_status, &unset_flag);
        /*
        Command *another = commands;
        int counter = handle_exit_status(commands);
        while (counter > 0 && another)
        {
            printf("command: %s exit_status: %i\n", *another->args, another->exit_status );
            another = another->next;
            counter--;

        }
        */
        Command *another = commands;
        int j = 0;
        if (another->next == NULL)
            e_status.last_exit_status = commands->exit_status;
        else
        {
            while(another != NULL && another->exit_status == 0)
            {
              //  printf("command: %s, exit_status: %i\n", another->args[0], another->exit_status);
                another = another->next;
            }
                //another=another->next;
            if (another == NULL)
                e_status.last_exit_status = 0;
            else
                e_status.last_exit_status = another->exit_status;
        }

        free_tokens(tokens);
        free_commands(commands);
        free(input);
    } while (status == 0);
    ft_free(&env_list);
    // Clean up built-in command names
    for (int i = 0; i < NUM_BUILTINS; i++)
        free(builtins[i].name);
}
