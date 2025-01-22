/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 15:54:00 by hawild            #+#    #+#             */
/*   Updated: 2025/01/17 18:55:32 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

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
# ifndef GLOBALS_H
#  define GLOBALS_H

extern int			g_execute_external_running;

# endif

# define CMD_TYPE_SIMPLE 1
# define CMD_TYPE_PIPELINE 2

typedef struct shell_status
{
	int				last_exit_status;
}					t_shell_status;

typedef struct command
{
	char			**args;
	char			*input_file;
	char			*output_file;
	int				append;
	struct command	*next;
	int				heredoc;
	int				exit_status;
	char			*heredoc_delim;
	char			*env_exp;
	int				output;
}					t_command;

typedef struct envvar
{
	char			*key;
	char			*value;
	struct envvar	*next;
}					t_envvar;

typedef struct builtin
{
	char			*name;
	int				(*func)(t_command *cmd, t_envvar **env_list);
}					t_builtin_cmd;

typedef struct token
{
	int				type;
	char			*value;
	struct token	*next;
}					t_token;

typedef struct s_tokenization_vars
{
	int				index;
	t_token			*head;
	t_token			*tail;
	char			*word;
}					t_tokenization_vars;

typedef struct pipeline_state
{
	int				prev_pipe_fd;
	int				last_pid;
	int				current_pipe_fd[2];
}					t_pipeline_state;

t_token				*process_whitespace(t_tokenization_vars *tvars,
						int *double_quote_count, char input_char);
t_token				*process_pipe(char **word, t_token **head, t_token **tail);
void				handle_new_token(t_token *new_t, char *input,
						int *double_quote_count, t_tokenization_vars *tvars);
void				process_input(char *input, t_tokenization_vars *tvars,
						int *double_quote_count);
void				process_special_tokens(char *input, int *double_quote_count,
						t_tokenization_vars *tvars);
t_token				*process_input_redirect(t_tokenization_vars *tvars,
						char *input);
t_token				*process_output_redirect(t_tokenization_vars *tvars,
						char *input);
t_token				*process_single_quote(t_tokenization_vars *tvars,
						char *input, int *double_quote_count);

void				handle_word_token(char **word, t_token **head,
						t_token **tail);
void				create_unique_temp_file(char *temp_file, int *fd);
void				wait_for_pipeline(int last_pid, t_shell_status *e_status);
t_command			*parse_commands(t_token **tokens, t_envvar **env_list,
						t_shell_status *e_status);
int					read_and_tokenize_input(char **input, t_token **tokens);
void				initialize_shell(t_envvar **env_list,
						t_builtin_cmd *builtins, t_shell_status *e_status);
void				cleanup_iteration(char *input, t_token *tokens,
						t_command *commands);
void				cleanup_shell(t_envvar **env_list, t_builtin_cmd *builtins);
size_t				ft_strlcpy(char *dest, const char *src, size_t n);
int					ft_execute_parsed_commands(t_command *cmd,
						t_builtin_cmd *builtins, t_envvar **env_list,
						t_shell_status *e_status);
int					execute_command_node(t_command *cmd,
						t_builtin_cmd *builtins, t_envvar **env_list);
void				handle_heredoc_to_fd(t_command *cmd);
void				handle_parent_process(pid_t pid, t_command *current_cmd,
						t_pipeline_state *ps);
void				handle_child_process(t_command *current_cmd,
						t_pipeline_state *ps, t_builtin_cmd *builtins,
						t_envvar **env_list);
void				handle_signal(int sig);
void				calculate(t_command *cmd);
char				**get_envp(t_envvar **env_list);
char				**convert_env_list_to_array(t_envvar **env_list);
int					populate_env_array(char **envp, t_envvar **env_list);
char				**allocate_env_array(int env_count);
int					count_env_vars(t_envvar **env_list);
t_token				*new_token(int type, char *value);
int					ft_error(t_command *cmd, const char *str);
void				update_oldpwd(t_envvar **env_list, const char *cwd);
void				update_pwd(t_envvar **env_list, const char *cwd);
int					handle_cd_home(t_command *cmd, t_envvar **env_list);
int					handle_cd_too_many_args(t_command *cmd);
int					is_valid(char *inp);
int					handle_newline_option(t_command *cmd, int *i);
int					open_output_file(t_command *cmd);
void				write_args_to_output(t_command *cmd, int fd,
						int start_index);
void				handle_newline_write(int newline, int fd);
void				echo_input_file(t_command *cmd);
t_token				*tokenize_input(char *input);
void				free_tokens(t_token *tokens);
void				append_token(t_token **head, t_token **tail,
						t_token *new_t);
void				append_str(char **dest, const char *src);
void				append_char(char **str, char c);
t_token				*handle_dollar(char *input, int *i);

t_command			*new_command(void);
void				add_argument(t_command *cmd, char *arg);
t_command			*parse_pipeline(t_token **tokens, t_envvar **env_list,
						t_shell_status *e_status);
void				free_commands(t_command *commands);
char				*ft_itoa(int n);
char				**ft_split(char *str);

void				display_env_vars(t_envvar **env_list);
void				add_to_list(t_envvar **env_list, char *key, char *value);
void				initialize_env(t_envvar **env_list);
void				add_or_update_var(t_envvar **env_list, char *key,
						char *value);
char				*get_env_var(t_envvar **env_list, char *key);
void				display_export_vars(t_envvar **env_list);
void				unset_env_var(t_envvar **env_list, char *key);
void				ft_free(t_envvar **env_list);
void				change_shell_lvl(t_envvar **env_list);

void				ft_init_builtins(t_builtin_cmd *builtins);
int					ft_run_builtin(t_builtin_cmd *builtins, char **args);
int					cmd_cd(t_command *cmd, t_envvar **env_list);
int					cmd_echo(t_command *cmd, t_envvar **env_list);
int					cmd_pwd(t_command *cmd, t_envvar **env_list);
int					cmd_export(t_command *cmd, t_envvar **env_list);
int					cmd_unset(t_command *cmd, t_envvar **env_list);
int					cmd_env(t_command *cmd, t_envvar **env_list);
int					cmd_exit(t_command *cmd, t_envvar **env_list);
int					ft_execute_builtin(t_command *cmd, t_builtin_cmd *builtins,
						t_envvar **env_list);
int					ft_execute_external(char **args, t_command *cmd,
						t_envvar **env_list);
char				*find_executable(char *command, t_envvar **env_list);
char				*ft_read_input(void);
char				**ft_split_input(char *input);
char				*ft_strjoin(char *str1, char *str2);
int					ft_execute_command(char **args, t_builtin_cmd *builtins);
void				ft_run_shell(t_envvar *env_list, t_builtin_cmd *builtins,
						t_command *commands, t_token *tokens);
char				*ft_strdup(const char *s);
void				handle_redirections(t_command *cmd);
void				expand_variables(t_command **cmds, t_envvar **env_list);
void				add_env_var(t_envvar **env_list, char *key, char *value);
int					check_permissions(char *filename);

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
