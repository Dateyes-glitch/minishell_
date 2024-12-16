#include "minishell.h"
#include "env_var.h"

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

int ft_execute_builtin(Command *cmd,builtin_cmd_t *builtins, envvar **env_list, unset_path_flag *unset_flag) 
{

    if (!cmd->args || !cmd->args[0]) 
        printf("Error: args is NULL or args[0] is NULL\n");
    int i = 0;
    while (i < NUM_BUILTINS)
    {
        if (strcmp(cmd->args[0], builtins[i].name) == 0)
        {
            builtins[i].func(cmd, env_list, unset_flag);
            return 0;
        }
        i++;
    }
    return 1;
}

int ft_execute_external(char **args, Command *cmd) {
    char *executable_path;
    extern char **environ;
    int exit_status = 0;
    if (strrchr(cmd->args[0],'/') != NULL)
    {
        char *save = strrchr(cmd->args[0], '/');
        char *dup = strdup(save);
        executable_path = find_executable(dup);
    }
    else
        executable_path = find_executable(args[0]);

    //char *executable_path = find_executable(args[0]);
    // printf("path: %s", executable_path);
    if (!executable_path) 
    {

        write(STDERR_FILENO, "minishell: command not found: ", 30);
        write(STDERR_FILENO, args[0], strlen(args[0]));
        write(STDERR_FILENO, "\n", 1);
        return 0;
    }

    pid_t pid = fork();
    if (pid == 0) 
    { 
        char **envp = environ;
        handle_redirections(cmd);
        execve(executable_path, args, envp);
        perror("execve"); 
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) 
    {
        wait(NULL);
      //  waitpid(pid, )
    }
    else 
        perror("minishell: fork");

    free(executable_path);
    return 0;
}


char *find_executable(char *command) 
{
    extern char **environ;
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
int cmd_cd(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) 
{
    if (cmd->args[1] == NULL)
        write(STDERR_FILENO, "minishell: expected argument to \"cd\"\n", 38);
    else 
    {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        envvar *oldpwd = *env_list;
        while (oldpwd && strcmp(oldpwd->key, "OLDPWD") != 0)
            oldpwd = oldpwd->next;
        oldpwd->value = strdup(cwd);
        oldpwd = NULL;
        free(oldpwd);
        if (chdir(cmd->args[1]) != 0)
            perror("minishell");
        getcwd(cwd, sizeof(cwd));
        envvar *pwd;
        pwd = *env_list;
        while(pwd && strcmp(pwd->key, "PWD") != 0)
            pwd = pwd->next;
        pwd->value = strdup(cwd);
        pwd = NULL;
        free(pwd);

    }
    return 1;
}


int cmd_echo(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) 
{
    int i = 1;
    int newline = 1;
    int fd = 1;

    if (cmd->output_file != NULL && cmd->append == 0)
        fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (cmd->append != 0)
        fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);

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


int cmd_pwd(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) 
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



int cmd_export(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) 
{
    int i;

    if (cmd->args[1] == NULL)
        display_export_vars(env_list);
    else
    {
        i = 0;
        char *equal_sign = strchr(cmd->args[1], '=');
        while (cmd->args[1][i] != '=')
            i++;
        cmd->args[1] = strndup(cmd->args[1], (size_t)i);
        char *value = equal_sign + 1;
        if (*value == '"')
        {
            value++;
            char *end_quote = strchr(value, '"');
            if (end_quote != NULL)
                *end_quote = '\0';
        }
        cmd->args[2] = strdup(value);
        cmd->args[3] = NULL;
        add_or_update_var(env_list, cmd->args[1], cmd->args[2],unset_flag);
    }
    return 1;
}



int cmd_unset(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) 
{
    int i;

    if (cmd->args[1] == NULL) 
        write(STDERR_FILENO, "minishell: unset: expected argument\n", 36);
    else if (cmd->args[2] == NULL)
        unset_env_var(env_list, cmd->args[1], unset_flag);
    else
    {
        i = 1;
        while (cmd->args[i] != NULL)
        {
            unset_env_var(env_list, cmd->args[i], unset_flag);
            i++;
        }
    }
    return 1;
}


int cmd_env(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) 
{
    int i;
    char *equal_sign;
    char *value;
    char *end_quote;

    if (cmd->args[1] == NULL)
        display_env_vars(env_list, unset_flag);
    else
    {
        equal_sign = strchr(cmd->args[1], '=');
        i = 0;
        while (cmd->args[1][i] != '=')
            i++;
        cmd->args[1] = strndup(cmd->args[1], (size_t)i);
        value = equal_sign + 1;
        if (*value == '=')
        {
            value++;
            end_quote = strchr(value, '"');
            if (end_quote != NULL)
                *end_quote = '\0';
        }
        cmd->args[2] = strdup(value);
        cmd->args[3] = NULL;
    }
    return 1;
}

int cmd_exit(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) 
{
    exit(0);
}

char *ft_read_input(void) 
{
    char *input = NULL;
    while (1)
    {
        input = readline("minishell> ");
        if (input == NULL)
            exit(0);
        if (*input == '\0')
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
