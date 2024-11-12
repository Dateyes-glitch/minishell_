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


void handle_redirections(Command *cmd)
{
    int fd_in = 0;
    int fd_out = 1;

    if (!cmd || !cmd->args) return;

    if (cmd->input_file) 
    {
        fd_in = open(cmd->input_file, O_RDONLY);
        if (fd_in < 0) 
        {
            perror("minishell: input file");
            exit(EXIT_FAILURE);
        }
        if (dup2(fd_in, STDIN_FILENO) < 0) 
        {
            perror("minishell: dup2 input");
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
            perror("minishell: output file");
            exit(EXIT_FAILURE);
        }
        if (dup2(fd_out, STDOUT_FILENO) < 0)
        {
            perror("minishell: dup2 output");
            close(fd_out);
            exit(EXIT_FAILURE);
        }
        close(fd_out);
    }
}


void handle_heredoc_to_fd(Command *cmd) 
{
    char *input = NULL;
    char *filename = strdup("heredoc_file.txt");
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    while (1)
    {
        input = readline("heredoc> ");

        if (input == NULL || strcmp(input, cmd->args[1]) == 0) 
        {
            if (strcmp(input, cmd->args[1]) == 0)
            write(fd, "\0", 1);
            cmd->input_file = filename;
            cmd->args[1] = NULL;
            cmd->args[0] = NULL;
            break;
        }

        write(fd, input, strlen(input));
        write(fd, "\n", 1);
        free(input);
    }

    if (!input)
        free(input);
    close(fd);
}

void dup_to_stdo(Command *cmd)
{
    char *com = "cat";
    char *executable_path = find_executable("cat");
    pid_t pid = fork();
    if (pid == 0) 
    { 
        char *envp[] = {NULL};
        handle_redirections(cmd);
        execve(executable_path, cmd->args,envp);
        perror("execve"); 
        exit(EXIT_FAILURE);
    } else if (pid > 0)
        wait(NULL);
    else
        perror("minishell: fork");

    free(executable_path);
}

int execute_command_node(Command *cmd, builtin_cmd_t *builtins, envvar **env_list, unset_path_flag *unset_flag) 
{
    if (ft_execute_builtin(cmd, builtins, env_list, unset_flag) == 0)
        return 0;
    else if (cmd->heredoc == 126)
    {
        handle_heredoc_to_fd(cmd);
        dup_to_stdo(cmd);
        return 0;
    }
    else
        return ft_execute_external(cmd->args, cmd);
}

void execute_pipeline(Command *cmd, builtin_cmd_t *builtins, envvar **env_list, shell_status *e_status, unset_path_flag *unset_flag) 
{
    int pipe_fd[2];
    int input_fd = STDIN_FILENO;
    pid_t pid;
    Command *current_cmd = cmd;
    int exit_status;
    int overall_status = 0;

    while (current_cmd) 
    {
        pipe(pipe_fd);
        if ((pid = fork()) == 0) 
        {
            dup2(input_fd, STDIN_FILENO);
            if (current_cmd->next)
                dup2(pipe_fd[1], STDOUT_FILENO);
            close(pipe_fd[0]);
            execute_command_node(current_cmd, builtins, env_list, unset_flag);
            exit(EXIT_SUCCESS);
        }
        else if (pid > 0)
        {
            waitpid(pid, &exit_status, 0);
            if (WIFEXITED(exit_status))
            {
                int cmd_exit_status = WEXITSTATUS(exit_status);
                printf("status in process: %d\n", cmd_exit_status);
                if (cmd_exit_status != 0)
                    printf("here\n");
                    overall_status = cmd_exit_status;
            }
            else
                overall_status = 1;
            close(pipe_fd[1]);
            input_fd = pipe_fd[0];
        }
        else
        {
            perror("minishell: fork");
            exit(EXIT_FAILURE);
        }
        current_cmd = current_cmd->next;
    }
    printf("status: %i\n", overall_status);
    e_status->last_exit_status = overall_status;
}

int ft_execute_parsed_commands(Command *cmd, builtin_cmd_t *builtins, envvar **env_list, shell_status *e_status, unset_path_flag *unset_flag) 
{
    if (!cmd) return 0;

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
    char *input;
    Token *tokens;
    Command *commands;
    int status = 0;
    builtin_cmd_t builtins[NUM_BUILTINS];
    envvar  *env_list = NULL;
    shell_status e_status;
    unset_path_flag unset_flag;

    unset_flag.flag = 0;
    initialize_env(&env_list);
    change_shell_lvl(&env_list);
    ft_init_builtins(builtins);

    signal(SIGINT, handle_signal);
    signal(SIGQUIT, handle_signal);

    do 
    {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            printf("%s%s%s %s%s%s : ", BOLD_GREEN, ARROW, RESET, UNDERLINE_B_I_MAGENTA, cwd, RESET);
        else
            perror("getcwd");

        input = ft_read_input();
        if (!input) break;

        tokens = tokenize_input(input);
        if (tokens == NULL || tokens->type == TOKEN_ERROR)
        {
            free(input);
            continue;
        }
        commands = parse_pipeline(&tokens, &env_list, &e_status);
        //expand_variables(&commands, &env_list);
        status = ft_execute_parsed_commands(commands, builtins, &env_list, &e_status, &unset_flag);

        free_tokens(tokens);
        free_commands(commands);
        free(input);
    } while (status == 0);
    ft_free(&env_list);
    // Clean up built-in command names
    for (int i = 0; i < NUM_BUILTINS; i++)
        free(builtins[i].name);
}
