#include "minishell.h"
#include "env_var.h"

int		execute_external_running = 0;

int	main(void)
{
	ft_run_shell();
	return (0);
}

char	*ft_strdup(const char *s)
{
	char	*d;

	d = malloc(strlen(s) + 1);
	if (d)
		strcpy(d, s);
	return (d);
}

void	ft_init_builtins(builtin_cmd_t *builtins)
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

int	ft_execute_builtin(Command *cmd, builtin_cmd_t *builtins, envvar **env_list,
		unset_path_flag *unset_flag)
{
	if (!cmd->args || !cmd->args[0])
		printf("Error: args is NULL or args[0] is NULL\n");
	int i = 0;
	while (i < NUM_BUILTINS)
	{
		if (strcmp(cmd->args[0], builtins[i].name) == 0)
		{
			builtins[i].func(cmd, env_list, unset_flag);
			return (0);
		}
		i++;
	}
	return (1);
}

int count_env_vars(envvar **env_list)
{
    int env_count = 0;
    envvar *current = *env_list;

    while (current != NULL)
    {
        env_count++;
        current = current->next;
    }
    return env_count;
}

char **allocate_env_array(int env_count)
{
    char **envp = malloc((env_count + 1) * sizeof(char *));
    if (!envp)
    {
        perror("malloc");
        return (NULL);
    }
    return (envp);
}

int populate_env_array(char **envp, envvar **env_list)
{
    envvar *current = *env_list;
    int i = 0;

    while (current != NULL)
    {
        size_t len = strlen(current->key) + strlen(current->value) + 2;
        envp[i] = malloc(len);
        if (!envp[i])
        {
            perror("malloc");
            return (-1);
        }
        snprintf(envp[i], len, "%s=%s", current->key, current->value);
        i++;
        current = current->next;
    }
    return (0);
}

char **convert_env_list_to_array(envvar **env_list)
{
    int env_count = count_env_vars(env_list);

    char **envp = allocate_env_array(env_count);
    if (!envp)
        return (NULL);

    if (populate_env_array(envp, env_list) == -1)
    {
        free(envp);
        return (NULL);
    }

    envp[env_count] = NULL;
    return (envp);
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char *get_executable_path(char **args, Command *cmd, envvar **env_list)
{
    char *executable_path;

    if (strrchr(cmd->args[0], '/') != NULL)
    {
        char *save = strrchr(cmd->args[0], '/');
        char *dup = strdup(save);
        executable_path = find_executable(dup, env_list);
        free(dup);
    }
    else
    {
        executable_path = find_executable(args[0], env_list);
    }
    return executable_path;
}

void handle_command_not_found(Command *cmd, char **args)
{
    if (cmd->env_exp != NULL)
    {
        printf("%s : is a directory\n", cmd->env_exp);
        cmd->exit_status = 126;
    }
    else
    {
        write(2, args[0], strlen(args[0]));
        write(STDERR_FILENO, ": command not found", 19);
        write(STDERR_FILENO, "\n", 1);
        cmd->exit_status = 127;
    }
}

char **get_envp(envvar **env_list)
{
    char **envp = convert_env_list_to_array(env_list);
    if (!envp)
        return (NULL);
    return (envp);
}

int execute_fork(char *executable_path, char **args, char **envp, Command *cmd)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        handle_redirections(cmd);
        execve(executable_path, args, envp);
        perror("execve");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        int status;
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid");
            cmd->exit_status = 1;
        }
        else
        {
            if (WIFEXITED(status))
                cmd->exit_status = WEXITSTATUS(status);
            else if (WIFSIGNALED(status))
                cmd->exit_status = 128 + WTERMSIG(status);
            else
                cmd->exit_status = 1;
        }
        return (0);
    }
    else
    {
        perror("minishell: fork");
        cmd->exit_status = 1;
        return (-1);
    }
}

int ft_execute_external(char **args, Command *cmd, envvar **env_list)
{
    char *executable_path = get_executable_path(args, cmd, env_list);
    if (!executable_path)
    {
        handle_command_not_found(cmd, args);
        return (0);
    }

    char **envp = get_envp(env_list);
    if (!envp)
    {
        cmd->exit_status = 1;
        free(executable_path);
        return (1);
    }

    execute_external_running = 1;
    int result = execute_fork(executable_path, args, envp, cmd);
    execute_external_running = 0;

    free(executable_path);
    return (result);
}

char *find_executable(char *command, envvar **env_list) 
{
    envvar *current = *env_list;
    char *path = NULL;

    while (current != NULL) {
        if (strcmp(current->key, "PATH") == 0) {
            path = current->value;
            break;
        }
        current = current->next;
    }

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

envvar	*find_env_var(envvar *env_list, const char *key)
{
	envvar *current = env_list;
	while (current != NULL)
	{
		if (strcmp(current->key, key) == 0)
		{
			return (current);
		}
		current = current->next;
	}
	return (NULL);
}

static void	update_oldpwd(envvar **env_list, const char *cwd,
		unset_path_flag *unset_flag)
{
	envvar	*var;

	var = find_env_var(*env_list, "OLDPWD");
	if (var)
	{
		free(var->value);
		var->value = strdup(cwd);
	}
	else
	{
		add_env_var(env_list, "OLDPWD", (char *)cwd, unset_flag);
		// Cast to char*
	}
}

static void	update_pwd(envvar **env_list, const char *cwd,
		unset_path_flag *unset_flag)
{
	envvar	*var;

	var = find_env_var(*env_list, "PWD");
	if (var)
	{
		free(var->value);
		var->value = strdup(cwd);
	}
	else
	{
		add_env_var(env_list, "PWD", (char *)cwd, unset_flag); // Cast to char*
	}
}

static int	handle_cd_home(Command *cmd, envvar **env_list)
{
	char	*home;

	home = getenv("HOME");
	if (home == NULL)
	{
		write(STDERR_FILENO, "minishell: cd: HOME not set\n", 28);
		cmd->exit_status = 1;
		return (1);
	}
	cmd->args[1] = home;
	return (0);
}

static int	handle_cd_too_many_args(Command *cmd)
{
	write(STDERR_FILENO, "minishell: cd: too many arguments\n", 35);
	cmd->exit_status = 1;
	return (1);
}

static int ft_error(Command *cmd, const char *str)
{
    perror(str);
	cmd->exit_status = 1;
	return (1);
}

int	cmd_cd(Command *cmd, envvar **env_list, unset_path_flag *unset_flag)
{
	char cwd[1024];

	if (cmd->args[1] == NULL)
	{
		if (handle_cd_home(cmd, env_list))
			return (1);
	}
	else if (cmd->args[2] != NULL)
		return (handle_cd_too_many_args(cmd));
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (ft_error(cmd, "minishell: cd: getcwd"));
	update_oldpwd(env_list, cwd, unset_flag);
	if (chdir(cmd->args[1]) != 0)
		return (ft_error(cmd, "minishell: cd"));
	if (getcwd(cwd, sizeof(cwd)) == NULL)
        return (ft_error(cmd, "minishell: cd: getcwd"));
	update_pwd(env_list, cwd, unset_flag);
	cmd->exit_status = 0;
	return (0);
}

static int handle_newline_option(Command *cmd, int *i)
{
    int newline = 1;
    if (cmd->args[1] && strcmp(cmd->args[1], "-n") == 0)
    {
        newline = 0;
        *i = 2;
    }
    return newline;
}

static int open_output_file(Command *cmd)
{
    int fd = STDOUT_FILENO;
    if (cmd->output_file != NULL)
    {
        if (cmd->append)
        {
            fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        }
        else
        {
            fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }

        if (fd == -1)
        {
            perror("minishell: echo");
            cmd->exit_status = 1;
        }
    }
    return fd;
}

static void write_args_to_output(Command *cmd, int fd, int start_index)
{
    for (int i = start_index; cmd->args[i] != NULL; i++)
    {
        if (i > start_index)
        {
            write(fd, " ", 1);
        }
        write(fd, cmd->args[i], strlen(cmd->args[i]));
    }
}

static void handle_newline_write(int newline, int fd)
{
    if (newline)
    {
        write(fd, "\n", 1);
    }
}

int cmd_echo(Command *cmd, envvar **env_list, unset_path_flag *unset_flag)
{
    int i = 1;
    int newline = handle_newline_option(cmd, &i);
    int fd = open_output_file(cmd);

    if (fd == -1)
    {
        return 1;
    }

    write_args_to_output(cmd, fd, i);
    handle_newline_write(newline, fd);

    if (fd != STDOUT_FILENO)
    {
        close(fd);
    }

    cmd->exit_status = 0;
    return 0;
}

int	cmd_pwd(Command *cmd, envvar **env_list, unset_path_flag *unset_flag)
{
	char cwd[1024];
	cmd->exit_status = 0;
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		write(STDOUT_FILENO, cwd, strlen(cwd));
		write(STDOUT_FILENO, "\n", 1);
	}
	else
		perror("getcwd");
	return (1);
}

int	is_valid(char *inp)
{
	char *cpy = strdup(inp);
	int i = 0;
	int j = 0;
	while (cpy[j] >= '0' && cpy[j] <= '9')
	{
		j++;
	}
	if (cpy[j] == '\0' || cpy[j] == '=')
		return (0);
	while (cpy[i] != '\0' && cpy[i] != '=')
	{
		if (isalnum(cpy[i]) != 0)
			i++;
		else
			cpy[i] = '\0';
	}
	if (i == strlen(inp) || cpy[i] == '=')
		return (1);
	else
		return (0);
}

static void	report_invalid_identifier(Command *cmd, char *arg)
{
	write(STDERR_FILENO, "minishell: export: `", 20);
	write(STDERR_FILENO, arg, strlen(arg));
	write(STDERR_FILENO, "': not a valid identifier\n", 26);
	cmd->exit_status = 1;
}
static void	handle_export_without_value(Command *cmd, envvar **env_list,
		unset_path_flag *unset_flag, char *arg)
{
	if (is_valid(arg))
	{
		add_or_update_var(env_list, arg, "", unset_flag);
	}
	else
	{
		report_invalid_identifier(cmd, arg);
	}
}

static void	handle_export_with_value(Command *cmd, envvar **env_list,
		unset_path_flag *unset_flag, char *arg, char *equal_sign)
{
	char	*key;
	char	*value;

	*equal_sign = '\0';
	key = arg;
	value = equal_sign + 1;
	if (is_valid(key))
	{
		add_or_update_var(env_list, key, value, unset_flag);
	}
	else
	{
		report_invalid_identifier(cmd, key);
	}
	*equal_sign = '=';
}

static void	process_export_argument(Command *cmd, envvar **env_list,
		unset_path_flag *unset_flag, char *arg)
{
	char	*equal_sign;

	equal_sign = strchr(arg, '=');
	if (equal_sign == NULL)
	{
		handle_export_without_value(cmd, env_list, unset_flag, arg);
	}
	else
	{
		handle_export_with_value(cmd, env_list, unset_flag, arg, equal_sign);
	}
}
int	cmd_export(Command *cmd, envvar **env_list, unset_path_flag *unset_flag)
{
	if (cmd->args[1] == NULL)
	{
		display_export_vars(env_list);
		cmd->exit_status = 0;
		return (0);
	}
	for (int i = 1; cmd->args[i] != NULL; i++)
	{
		process_export_argument(cmd, env_list, unset_flag, cmd->args[i]);
	}
	return (0);
}

int	cmd_unset(Command *cmd, envvar **env_list, unset_path_flag *unset_flag)
{
    int i;

	if (cmd->args[1] == NULL)
	{
		cmd->exit_status = 0;
		return (0);
	}
    i = 1;
	while (cmd->args[i] != NULL)
	{
		unset_env_var(env_list, cmd->args[i], unset_flag);
        i++;
	}
	cmd->exit_status = 0;
	return (0);
}
int	cmd_env(Command *cmd, envvar **env_list, unset_path_flag *unset_flag)
{
	if (cmd->args[1] != NULL)
	{
		write(STDERR_FILENO, "minishell: env: too many arguments\n", 35);
		cmd->exit_status = 1;
		return (1);
	}
	display_env_vars(env_list, unset_flag);
	cmd->exit_status = 0;
	return (0);
}

int	is_string(char *str)
{
	char	*cpy;
	int		i;
	int		j;

	cpy = strdup(str);
	i = 0;
	j = 0;
	while (cpy[i])
	{
		if (isalpha(cpy[i]))
			j++;
		i++;
	}
	free(cpy);
	if (j != 0)
		return (j);
	return (0);
}

int	cmd_exit(Command *cmd, envvar **env_list, unset_path_flag *unset_flag)
{
    char *endptr;
    long exit_code;

	if (cmd->args[1] == NULL)
	{
		exit(0);
	}
	if (cmd->args[2] != NULL)
	{
		write(STDERR_FILENO, "minishell: exit: too many arguments\n", 36);
		cmd->exit_status = 1;
		return (1);
	}
	exit_code = strtol(cmd->args[1], &endptr, 10);
	if (*endptr != '\0' || exit_code > INT_MAX || exit_code < INT_MIN)
	{
		write(STDERR_FILENO, "minishell: exit: ", 17);
		write(STDERR_FILENO, cmd->args[1], strlen(cmd->args[1]));
		write(STDERR_FILENO, ": numeric argument required\n", 28);
		exit(2);
	}
	exit((int)exit_code);
}

char	*ft_read_input(void)
{
	char *input = readline("minishell>");
	if (input && *input)
	{
		add_history(input);
	}
	return (input);
}
