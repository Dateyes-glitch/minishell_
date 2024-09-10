#include "minishell.h"

int main() 
{
    ft_run_shell();
    return 0;
}


char *ft_strdup(const char *s) 
{
    char *d = malloc(strlen(s) + 1);
    if (d) strcpy(d, s);
    return d;
}

void ft_init_builtins(builtin_cmd_t *builtins) 
{
    builtins[0].name = ft_strdup("cd");
    builtins[0].func = cmd_cd;
    builtins[1].name = ft_strdup("echo");
    builtins[1].func = cmd_echo;
    builtins[2].name = ft_strdup("pwd");
    builtins[2].func = cmd_pwd;
    builtins[3].name = ft_strdup("export");
    builtins[3].func = cmd_export;
    builtins[4].name = ft_strdup("unset");
    builtins[4].func = cmd_unset;
    builtins[5].name = ft_strdup("env");
    builtins[5].func = cmd_env;
    builtins[6].name = ft_strdup("exit");
    builtins[6].func = cmd_exit;
}

int ft_execute_builtin(Command *cmd,builtin_cmd_t *builtins) 
{

    if (!cmd->args || !cmd->args[0]) 
        printf("Error: args is NULL or args[0] is NULL\n");
    int i = 0;
    while (i < NUM_BUILTINS)
    {
        if (strcmp(cmd->args[0], builtins[i].name) == 0)
        {
            builtins[i].func(cmd);
            return 1;
        }
        i++;
    }
    return 0;
}

int ft_execute_external(char **args, Command *cmd) {
    char *executable_path = find_executable(args[0]);
    // printf("path: %s", executable_path);
    if (!executable_path) 
    {

        write(STDERR_FILENO, "minishell: command not found: ", 30);
        write(STDERR_FILENO, args[0], strlen(args[0]));
        write(STDERR_FILENO, "\n", 1);
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) 
    { 
        char *envp[] = {NULL};
        handle_redirections(cmd);
        execve(executable_path, args, envp);
        perror("execve"); 
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) 
        wait(NULL);
    else 
        perror("minishell: fork");

    free(executable_path);
    return 1;
}


char *find_executable(char *command) {
    char *path = getenv("PATH");
    if (!path) return NULL;

    char *path_copy = ft_strdup(path);
    if (!path_copy) return NULL;

    char *dir_start = path_copy;
    char *dir_end;
    while ((dir_end = strchr(dir_start, ':')) != NULL) 
    {
        *dir_end = '\0';
        char *full_path = malloc(strlen(dir_start) + strlen(command) + 2);
        if (!full_path) {
            free(path_copy);
            return NULL;
        }

        sprintf(full_path, "%s/%s", dir_start, command);

        if (access(full_path, X_OK) == 0) 
        {
            free(path_copy);
            return full_path;
        }

        free(full_path);
        dir_start = dir_end + 1;
    }

    char *full_path = malloc(strlen(dir_start) + strlen(command) + 2);
    if (full_path) 
    {
        sprintf(full_path, "%s/%s", dir_start, command);
        if (access(full_path, X_OK) == 0) 
        {
            free(path_copy);
            return full_path;
        }
        free(full_path);
    }

    free(path_copy);
    return NULL;
}


// Built-in command: cd
int cmd_cd(Command *cmd) 
{
    if (cmd->args[1] == NULL)
        write(STDERR_FILENO, "minishell: expected argument to \"cd\"\n", 38);
    else 
    {
        if (chdir(cmd->args[1]) != 0)
            perror("minishell");
    }
    return 1;
}


int cmd_echo(Command *cmd) {
    int i = 1;
    int newline = 1;
    int fd = 1;

    if (cmd->output_file != NULL)
        fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (cmd->args[1] && strcmp(cmd->args[1], "-n") == 0) 
    {
        newline = 0;
        i = 2;
    }

    for (; cmd->args[i] != NULL; i++) 
    {
        if (i > 1) write(fd, " ", 1); 
        write(fd, cmd->args[i], strlen(cmd->args[i]));
    }
    
    if (newline) write(fd, "\n", 1);

    return 1;
}


int cmd_pwd(Command *cmd) 
{
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) != NULL) 
    {
        write(STDOUT_FILENO, cwd, strlen(cwd));
        write(STDOUT_FILENO, "\n", 1);
    } 
    else
        perror("getcwd");
    return 1;
}



int cmd_export(Command *cmd) 
{

    if (cmd->args[1] == NULL)
        write(STDERR_FILENO, "minishell: export: expected argument\n", 37);
    else
        putenv(cmd->args[1]);
    return 1;
}



int cmd_unset(Command *cmd) 
{

    if (cmd->args[1] == NULL) 
        write(STDERR_FILENO, "minishell: unset: expected argument\n", 36);
    else 
        unsetenv(cmd->args[1]);
    return 1;
}


int cmd_env(Command *cmd) 
{
    extern char **environ;
    for (char **env = environ; *env; ++env) 
    {
        write(STDOUT_FILENO, *env, strlen(*env));
        write(STDOUT_FILENO, "\n", 1);
    }
    return 1;
}

int cmd_exit(Command *cmd) 
{
    exit(0);
}

char *ft_read_input(void) 
{
    char *input = NULL;
    while (1)
    {
        input = readline("minishell> ");
        if (input == NULL || *input == '\0')
        {
            rl_on_new_line();
            rl_replace_line("", 0);
            rl_redisplay();
        }
        if (*input)
        {
            add_history(input);
            return input;
        }
        free(input);
    }
}
