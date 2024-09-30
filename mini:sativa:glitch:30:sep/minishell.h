#ifndef MINISHELL_H
# define MINISHELL_H

#include "env_var.h"
#include "tokens.h"
#include "cmds.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>



#define NUM_BUILTINS 7
#define BUF_SIZE 64

typedef struct envvar 
{
    char *key;
    char *value;
    struct envvar *next;
}   envvar;

typedef struct builtin
{
  char *name;
  int(*func)(Command *cmd, envvar **env_list);
} builtin_cmd_t;


void    display_env_vars(envvar **env_list);
void    add_to_list(envvar **env_list, char *key, char *value);
void    initialize_env(envvar **env_list);
void    add_or_update_var(envvar **env_list, char *key, char *value);
char    *get_env_var(envvar **env_list, char *key);
void    display_export_vars(envvar **env_list);
void    unset_env_var(envvar **env_list, char *key);
void    ft_free(envvar **env_list);
void    change_shell_lvl(envvar **env_list);


void ft_init_builtins(builtin_cmd_t *builtins);
int ft_run_builtin(builtin_cmd_t *builtins, char **args);
int cmd_cd(Command *cmd, envvar **env_list);
int cmd_echo(Command *cmd, envvar **env_list);
int cmd_pwd(Command *cmd, envvar **env_list);
int cmd_export(Command *cmd, envvar **env_list);
int cmd_unset(Command *cmd, envvar **env_list);
int cmd_env(Command *cmd, envvar **env_list);
int cmd_exit(Command *cmd, envvar **env_list);
int ft_execute_builtin(Command *cmd, builtin_cmd_t *builtins, envvar **env_list);
int ft_execute_external(char **args, Command *cmd);
char *find_executable(char *command);
char *ft_read_input(void);
char **ft_split_input(char *input);
int ft_execute_command(char **args, builtin_cmd_t *builtins);
void ft_run_shell(void);
char *ft_strdup(const char *s);
void handle_redirections(Command *cmd);

# define    RESET      "\033[0m"
# define    ITALIC       "\033[3m"
# define    BOLD_ITALIC_MAGENTA "\033[1;3;35m"
# define    UNDERLINE  "\033[4m"
# define    UNDERLINE_B_I_MAGENTA "\033[1;3;4;35m"
# define    BOLD_GREEN "\033[1;32m"
# define    BOLD_BLUE   "\033[1;34m"
# define    BOLD_CYAN   "\033[1;36m"
# define    ARROW       "➜ "

#endif
