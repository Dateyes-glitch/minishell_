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

int execute_command_node(Command *cmd, builtin_cmd_t *builtins, envvar **env_list) 
{
    if (ft_execute_builtin(cmd, builtins, env_list))
        return 1;
    else if (cmd->heredoc == 126)
    {
        handle_heredoc_to_fd(cmd);
        dup_to_stdo(cmd);
        return 1;
    }
    else
        return ft_execute_external(cmd->args, cmd);
}

void execute_pipeline(Command *cmd, builtin_cmd_t *builtins, envvar **env_list) 
{
    int pipe_fd[2];
    int input_fd = STDIN_FILENO;
    pid_t pid;
    Command *current_cmd = cmd;

    while (current_cmd) 
    {
        pipe(pipe_fd);
        if ((pid = fork()) == 0) 
        {
            dup2(input_fd, STDIN_FILENO);
            if (current_cmd->next)
                dup2(pipe_fd[1], STDOUT_FILENO);
            close(pipe_fd[0]);
            execute_command_node(current_cmd, builtins, env_list);
            exit(EXIT_SUCCESS);
        }
        else if (pid > 0)
        {
            wait(NULL);
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
}

int ft_execute_parsed_commands(Command *cmd, builtin_cmd_t *builtins, envvar **env_list) 
{
    if (!cmd) return 1;

    if (!cmd->next) 
        return execute_command_node(cmd, builtins, env_list); 
    else 
    {
        execute_pipeline(cmd, builtins, env_list);
        return 1;
    }
}

void ft_run_shell(void) 
{
    char *input;
    Token *tokens;
    Command *commands;
    int status = 1;
    builtin_cmd_t builtins[NUM_BUILTINS];
    envvar  *env_list = NULL;
    // list *environment = init_env();
    initialize_env(&env_list);
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
        if (tokens->type == TOKEN_ERROR)
        {
            free(input);
            continue;
        }
        commands = parse_pipeline(&tokens);
        status = ft_execute_parsed_commands(commands, builtins, &env_list);

        free_tokens(tokens);
        free_commands(commands);
        free(input);
    } while (status);

    // Clean up built-in command names
    for (int i = 0; i < NUM_BUILTINS; i++)
        free(builtins[i].name);
}
