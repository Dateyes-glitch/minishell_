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
// int ft_execute_external(char **args, Command *cmd) {
//     char *executable_path;
//     extern char **environ;

//     if (cmd->args[0][0] == '.' && cmd->args[0][1] == '/')
//         executable_path = strdup(cmd->args[0]);
//     else if (strrchr(cmd->args[0], '/') != NULL) {
//         char *save = strrchr(cmd->args[0], '/');
//         char *dup = strdup(save);
//         executable_path = find_executable(dup);
//         free(dup);
//     } else {
//         executable_path = find_executable(args[0]);
//     }
//     //check permissions and if can access file in dir if yes execve else permissions.
//     if (!executable_path) {
//         if (cmd->env_exp != NULL)
//         {
//             printf("%s : is a directory\n", cmd->env_exp);
//             cmd->exit_status = 126;
//         }
//         else
//         {
//             write(2, args[0], strlen(args[0]));
//             write(STDERR_FILENO, ": command not found", 19);
//             write(STDERR_FILENO, "\n", 1);
//             cmd->exit_status = 127;

//         }
//         return 0;
//     }

//     pid_t pid = fork();
//     if (pid == 0) {
//         // Child process
//         char **envp = environ;
//         handle_redirections(cmd);
//         execve(executable_path, args, envp);
//         perror("execve");
//         exit(EXIT_FAILURE); // Exit with failure if execve fails
//     } else if (pid > 0) {
//         // Parent process
//         int status;
//         if (waitpid(pid, &status, 0) == -1) {
//             perror("waitpid");
//             cmd->exit_status = 1; // Set a generic error code
//         } else {
//             if (WIFEXITED(status)) {
//                 cmd->exit_status = WEXITSTATUS(status);
//             } else if (WIFSIGNALED(status)) {
//                 cmd->exit_status = 128 + WTERMSIG(status);
//             } else {
//                 cmd->exit_status = 1; // Set a generic error code for unexpected cases
//             }
//         }
//     } else {
//         // Fork failed
//         perror("minishell: fork");
//         cmd->exit_status = 1; // Set a generic error code for fork failure
//     }

//     free(executable_path);
//     return 0;
// }


// char *find_executable(char *command) 
// {
//     extern char **environ;
//     char *path = getenv("PATH");
//     if (!path) return NULL;

//     char *path_copy = ft_strdup(path);
//     if (!path_copy) return NULL;

//     char *dir_start = path_copy;
//     char *dir_end;
//     while ((dir_end = strchr(dir_start, ':')) != NULL) 
//     {
//         *dir_end = '\0';
//         char *full_path = malloc(strlen(dir_start) + strlen(command) + 2);
//         if (!full_path) {
//             free(path_copy);
//             return NULL;
//         }

//         sprintf(full_path, "%s/%s", dir_start, command);

//         if (access(full_path, X_OK) == 0) 
//         {
//             free(path_copy);
//             return full_path;
//         }

//         free(full_path);
//         dir_start = dir_end + 1;
//     }

//     char *full_path = malloc(strlen(dir_start) + strlen(command) + 2);
//     if (full_path) 
//     {
//         sprintf(full_path, "%s/%s", dir_start, command);
//         if (access(full_path, X_OK) == 0) 
//         {
//             free(path_copy);
//             return full_path;
//         }
//         free(full_path);
//     }

//     free(path_copy);
//     return NULL;
// }

char **convert_env_list_to_array(envvar **env_list) {
    // Count the number of environment variables in the list
    int env_count = 0;
    envvar *current = *env_list;
    while (current != NULL) {
        env_count++;
        current = current->next;
    }

    // Allocate memory for the char ** array
    char **envp = malloc((env_count + 1) * sizeof(char *));
    if (!envp) {
        perror("malloc");
        return NULL;  // Return NULL on failure
    }

    // Populate the char ** array with environment variables as strings
    current = *env_list;
    int i = 0;
    while (current != NULL) {
        size_t len = strlen(current->key) + strlen(current->value) + 2; // +2 for '=' and '\0'
        envp[i] = malloc(len);
        if (!envp[i]) {
            perror("malloc");
            return NULL;  // Return NULL on failure
        }
        snprintf(envp[i], len, "%s=%s", current->key, current->value);
        i++;
        current = current->next;
    }
    envp[env_count] = NULL;  // Null-terminate the array for execve
    return envp;
}



int ft_execute_external(char **args, Command *cmd, envvar **env_list) {
    char *executable_path;
  //  extern char **environ;

    if (strrchr(cmd->args[0], '/') != NULL) {
        char *save = strrchr(cmd->args[0], '/');
        char *dup = strdup(save);
        executable_path = find_executable(dup, env_list);
        free(dup);
    } else {
        executable_path = find_executable(args[0], env_list);
    }
    //check permissions and if can access file in dir if yes execve else permissions.
    if (!executable_path) {
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
        return 0;
    }
    // Convert env_list (linked list) to char ** for execve
        char **envp = convert_env_list_to_array(env_list);
        if (!envp) {
            cmd->exit_status = 1; // Error converting environment
            return 1;
        }
        int i = 0;
        while (envp[i])
        {
            printf("%s\n", envp[i]);
            i++;
        }
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
       // char **envp = environ;
      //  char **envp = env_list;

        
        handle_redirections(cmd);
        execve(executable_path, args, envp);
        perror("execve");
        exit(EXIT_FAILURE); // Exit with failure if execve fails
    } else if (pid > 0) {
        // Parent process
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            cmd->exit_status = 1; // Set a generic error code
        } else {
            if (WIFEXITED(status)) {
                cmd->exit_status = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                cmd->exit_status = 128 + WTERMSIG(status);
            } else {
                cmd->exit_status = 1; // Set a generic error code for unexpected cases
            }
        }
    } else {
        // Fork failed
        perror("minishell: fork");
        cmd->exit_status = 1; // Set a generic error code for fork failure
    }

    free(executable_path);
    return 0;
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



envvar *find_env_var(envvar *env_list, const char *key) {
    envvar *current = env_list;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current; 
        }
        current = current->next;
    }
    return NULL; // Variable not found
}

int cmd_cd(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) {
    char *home = NULL;
    char cwd[1024];
    envvar *var;

    if (cmd->args[1] == NULL) {
        home = getenv("HOME");
        if (home == NULL) {
            write(STDERR_FILENO, "minishell: cd: HOME not set\n", 28);
            cmd->exit_status = 1;
            return 1; 
        }
        cmd->args[1] = home; 
    } else if (cmd->args[2] != NULL) {
        write(STDERR_FILENO, "minishell: cd: too many arguments\n", 35);
        cmd->exit_status = 1;
        return 1; 
    }

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("minishell: cd: getcwd");
        cmd->exit_status = 1;
        return 1;
    }

    // Update OLDPWD
    var = find_env_var(*env_list, "OLDPWD");
    if (var) {
        free(var->value);
        var->value = strdup(cwd); 
    } else {
        add_env_var(env_list, "OLDPWD", cwd, unset_flag);
    }

    if (chdir(cmd->args[1]) != 0) {
        perror("minishell: cd");
        cmd->exit_status = 1;
        return 1; 
    }

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("minishell: cd: getcwd");
        cmd->exit_status = 1;
        return 1;
    }

    // Update PWD
    var = find_env_var(*env_list, "PWD");
    if (var) {
        free(var->value);
        var->value = strdup(cwd); 
    } else {
        add_env_var(env_list, "PWD", cwd, unset_flag);
    }

    cmd->exit_status = 0;
    return 0; 
}

int cmd_echo(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) {
    int i = 1;
    int newline = 1;
    int fd = STDOUT_FILENO; // Default to standard output

    if (cmd->args[1] && strcmp(cmd->args[1], "-n") == 0) {
        newline = 0;
        i = 2;
    }

    // Handle output redirection
    if (cmd->output_file != NULL) {
        if (cmd->append) {
            fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        } else {
            fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }

        if (fd == -1) {
            perror("minishell: echo");
            cmd->exit_status = 1;
            return 1;
        }
    }

    // Ignore input redirection (consistent with bash)
    // ... (no input file handling here) 

    // Normal echo behavior
    for (; cmd->args[i] != NULL; i++) {
        if (i > 1) {
            write(fd, " ", 1);
        }
        write(fd, cmd->args[i], strlen(cmd->args[i]));
    }

    if (newline) {
        write(fd, "\n", 1);
    }

    if (fd != STDOUT_FILENO) {
        close(fd); 
    }

    cmd->exit_status = 0;
    return 0;
}


int cmd_pwd(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) 
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
    return 1;
}

int     is_valid(char *inp)
{
    char *cpy = strdup(inp);
    int i = 0;
    int j = 0;
    while (cpy[j] >= '0' && cpy[j] <= '9') {j++;}
    if (cpy[j] == '\0' || cpy[j] == '=')
        return 0;
    while (cpy[i] != '\0' && cpy[i] != '=')
    {
        if (isalnum(cpy[i]) != 0 )
            i++;
        else
            cpy[i] = '\0';
    }


    if (i == strlen(inp) || cpy[i] == '=')
        return 1;
    else
        return 0;
}


int cmd_export(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) {
    if (cmd->args[1] == NULL) {
        display_export_vars(env_list);
        cmd->exit_status = 0;
        return 0; 
    }

    for (int i = 1; cmd->args[i] != NULL; i++) {
        char *arg = cmd->args[i];
        char *equal_sign = strchr(arg, '=');

        if (equal_sign == NULL) { 
            if (is_valid(arg)) {
                add_or_update_var(env_list, arg, "", unset_flag); 
            } else {
                write(STDERR_FILENO, "minishell: export: `", 20);
                write(STDERR_FILENO, arg, strlen(arg));
                write(STDERR_FILENO, "': not a valid identifier\n", 26);
                cmd->exit_status = 1; 
            }
        } else {
            *equal_sign = '\0'; // Temporarily null-terminate for key extraction
            char *key = arg;
            char *value = equal_sign + 1;

            if (is_valid(key)) {
                add_or_update_var(env_list, key, value, unset_flag);
            } else {
                write(STDERR_FILENO, "minishell: export: `", 20);
                write(STDERR_FILENO, key, strlen(key));
                write(STDERR_FILENO, "': not a valid identifier\n", 26);
                cmd->exit_status = 1; 
            }
            *equal_sign = '='; // Restore the equal sign
        }
    }

    return 0; 
}
// int cmd_export(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) 
// {
//     int i;

//     char *bef = NULL;
//     char *after = NULL;
//     char *exists = NULL;
//     if (cmd->args[1] != NULL)
//     {
//         exists = strchr(cmd->args[1], '=');
//         bef = (strchr(cmd->args[1], '=') - 1);
//         after = (strchr(cmd->args[1], '=') + 1);
        
//     }
//     if (cmd->args[1] == NULL)
//         display_export_vars(env_list);
//     else if (exists == NULL && is_valid(cmd->args[1]) == 1)
//             add_or_update_var(env_list, cmd->args[1], NULL, unset_flag);
//     else if ( cmd->args[1][0] == '='||is_valid(cmd->args[1]) == 0 || (bef != NULL && *after != '\0' && (isalnum(bef[0]) == 0 || isalnum((after[0]) == 0))))
//     {
//         write(2,"minishell: ", 11);
//         write(2, "export: ", 9);
//         write(2, "`", 1);
//         write(2, cmd->args[1], strlen(cmd->args[1]));
//         write(2, "'", 1);
//         write(2, ": not a valid identifier\n", 26);
//         cmd->exit_status = 1;
//     }
//     else
//     {
//         i = 0;
//         char *equal_sign = strchr(cmd->args[1], '=');
//         while (cmd->args[1][i] != '=')
//             i++;
//         cmd->args[1] = strndup(cmd->args[1], (size_t)i);
//         char *value = equal_sign + 1;
//         if (*value == '"')
//         {
//             value++;
//             char *end_quote = strchr(value, '"');
//             if (end_quote != NULL)
//                 *end_quote = '\0';
//         }
//         cmd->args[2] = strdup(value);
//         cmd->args[3] = NULL;
//         add_or_update_var(env_list, cmd->args[1], cmd->args[2],unset_flag);
//         cmd->exit_status = 0; 
//     }
//     return 1;
// }

int cmd_unset(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) {
    if (cmd->args[1] == NULL) {
        cmd->exit_status = 0; 
        return 0; 
    }

    for (int i = 1; cmd->args[i] != NULL; i++) {
        unset_env_var(env_list, cmd->args[i], unset_flag);
    }

    cmd->exit_status = 0; 
    return 0;
}


int cmd_env(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) {
    if (cmd->args[1] != NULL) {
        write(STDERR_FILENO, "minishell: env: too many arguments\n", 35);
        cmd->exit_status = 1;
        return 1; 
    }
    display_env_vars(env_list, unset_flag);
    cmd->exit_status = 0; 
    return 0; 
}

int is_string(char *str)
{
    char *cpy = strdup(str);
    int i = 0;
    int j = 0;
    while (cpy[i])
    {
        if (isalpha(cpy[i]))
            j++;
        i++;
    }
    free(cpy);
    if (j != 0)
        return j;
    return 0;
}

int cmd_exit(Command *cmd, envvar **env_list, unset_path_flag *unset_flag) {
    if (cmd->args[1] == NULL) {
        exit(0); 
    }

    if (cmd->args[2] != NULL) {
        write(STDERR_FILENO, "minishell: exit: too many arguments\n", 36);
        cmd->exit_status = 1;
        return 1; 
    }

    char *endptr;
    long exit_code = strtol(cmd->args[1], &endptr, 10); 

    if (*endptr != '\0' || exit_code > INT_MAX || exit_code < INT_MIN) {
        write(STDERR_FILENO, "minishell: exit: ", 17);
        write(STDERR_FILENO, cmd->args[1], strlen(cmd->args[1]));
        write(STDERR_FILENO, ": numeric argument required\n", 28);
        exit(2);  
    }

    exit((int)exit_code); 
}


/*
int cmd_exit(Command *cmd, envvar **env_list, unset_path_flag *unset_flag)
{
    int exit_code = 0; // Default exit code

    // Case 1: No argument provided
    if (cmd->args[1] == NULL)
        exit(0); // Exit with code 0 if no argument is provided

    // Case 2: Argument provided is numeric
    char *arg = cmd->args[1];
    for (int i = 0; arg[i]; i++)
    {
        if ((arg[i] < '0' || arg[i] > '9') && !(i == 0 && arg[i] == '-'))
        {
            // Non-numeric argument detected
            write(2, "exit: ", 6);
            write(2, arg, strlen(arg));
            write(2, ": numeric argument required\n", 28);
            exit(2); // Exit with code 2 for non-numeric argument
        }
    }

    // Convert argument to integer (basic atoi implementation)
    exit_code = atoi(cmd->args[1]);

    // Case 3: Too many arguments
    if (cmd->args[2] != NULL)
    {
        write(2, "exit: too many arguments\n", 25);
        cmd->exit_status = 1; // Set exit status to 1
        return 1; // Return without exiting
    }

    // Exit with the provided exit code
    exit(exit_code);
}
*/

char *ft_read_input(void) 
{
    char *input = readline("minishell>");
    if (input && *input) 
    {
        add_history(input); 
    }
    return input;
}
