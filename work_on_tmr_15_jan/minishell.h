#ifndef MINISHELL_H
# define MINISHELL_H

// # include "cmds.h"
// # include "env_var.h"
// # include "tokens.h"
# include <ctype.h>
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>

# define TOKEN_WORD 1
# define TOKEN_PIPE 2
# define TOKEN_INPUT_REDIRECT 3
# define TOKEN_OUTPUT_REDIRECT 4
# define TOKEN_APPEND_REDIRECT 5
# define TOKEN_HEREDOC 6
# define TOKEN_QUOTE 7
# define TOKEN_DOUBLE_QUOTE 8
# define TOKEN_ENV_VAR 9
# define TOKEN_EXIT_STATUS 10
# define TOKEN_EOF 11
# define TOKEN_ERROR 12
# define TOKEN_MERGE_FLAG 13
# define TOKEN_DOUBLE_MERGE 14

# define NUM_BUILTINS 7
# define BUF_SIZE 64

extern int			execute_external_running;

# define CMD_TYPE_SIMPLE 1
# define CMD_TYPE_PIPELINE 2

typedef struct unset_path_flag
{
	int				flag;
}					unset_path_flag;

typedef struct shell_status
{
	int				last_exit_status;
}					shell_status;

typedef struct Command
{
	char			**args;
	char			*input_file;
	char			*output_file;
	int append;
	struct Command	*next;
	int				heredoc;
	int				exit_status;
	char			*heredoc_delim;
	char			*env_exp;
	int				output;
}					Command;

typedef struct envvar
{
	char			*key;
	char			*value;
	struct envvar	*next;
}					envvar;

typedef struct builtin
{
	char			*name;
	int				(*func)(Command *cmd, envvar **env_list,
						unset_path_flag *unset_flag);
}					builtin_cmd_t;

typedef struct Token
{
	int				type;
	char			*value;
	struct Token	*next;
}					Token;

Token				*new_token(int type, char *value);
Token				*tokenize_input(char *input);
void				free_tokens(Token *tokens);
void				append_token(Token **head, Token **tail, Token *new_t);
void				append_str(char **dest, const char *src);
void				append_char(char **str, char c);
Token				*handle_dollar(char *input, int *i);

Command				*new_command(void);
void				add_argument(Command *cmd, char *arg);
Command				*parse_pipeline(Token **tokens, envvar **env_list,
						shell_status *e_status);
void				free_commands(Command *commands);
char				*ft_itoa(int n);
char				**ft_split(char *str);

void				display_env_vars(envvar **env_list,
						unset_path_flag *unset_flag);
void				add_to_list(envvar **env_list, char *key, char *value);
void				initialize_env(envvar **env_list);
void				add_or_update_var(envvar **env_list, char *key, char *value,
						unset_path_flag *unset_flag);
char				*get_env_var(envvar **env_list, char *key);
void				display_export_vars(envvar **env_list);
void				unset_env_var(envvar **env_list, char *key,
						unset_path_flag *unset_flag);
void				ft_free(envvar **env_list);
void				change_shell_lvl(envvar **env_list);

void				ft_init_builtins(builtin_cmd_t *builtins);
int					ft_run_builtin(builtin_cmd_t *builtins, char **args);
int					cmd_cd(Command *cmd, envvar **env_list,
						unset_path_flag *unset_flag);
int					cmd_echo(Command *cmd, envvar **env_list,
						unset_path_flag *unset_flag);
int					cmd_pwd(Command *cmd, envvar **env_list,
						unset_path_flag *unset_flag);
int					cmd_export(Command *cmd, envvar **env_list,
						unset_path_flag *unset_flag);
int					cmd_unset(Command *cmd, envvar **env_list,
						unset_path_flag *unset_flag);
int					cmd_env(Command *cmd, envvar **env_list,
						unset_path_flag *unset_flag);
int					cmd_exit(Command *cmd, envvar **env_list,
						unset_path_flag *unset_flag);
int					ft_execute_builtin(Command *cmd, builtin_cmd_t *builtins,
						envvar **env_list, unset_path_flag *unset_flag);
int					ft_execute_external(char **args, Command *cmd,
						envvar **env_list);
char				*find_executable(char *command, envvar **env_list);
char				*ft_read_input(void);
char				**ft_split_input(char *input);
char				*ft_strjoin(char *str1, char *str2);
int					ft_execute_command(char **args, builtin_cmd_t *builtins);
void				ft_run_shell(void);
char				*ft_strdup(const char *s);
void				handle_redirections(Command *cmd);
void				expand_variables(Command **cmds, envvar **env_list);
void				add_env_var(envvar **env_list, char *key, char *value,
						unset_path_flag *unset_flag);
int check_permissions(char *filename);

# define RESET "\033[0m"
# define ITALIC "\033[3m"
# define BOLD_ITALIC_MAGENTA "\033[1;3;35m"
# define UNDERLINE "\033[4m"
# define UNDERLINE_B_I_MAGENTA "\033[1;3;4;35m"
# define BOLD_GREEN "\033[1;32m"
# define BOLD_BLUE "\033[1;34m"
# define BOLD_CYAN "\033[1;36m"
# define ARROW "➜ "

#endif
