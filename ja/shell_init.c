/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 23:00:28 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:42:42 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	initialize_shell(envvar **env_list, builtin_cmd_t *builtins,
		shell_status *e_status, unset_path_flag *unset_flag)
{
	e_status->last_exit_status = 0;
	unset_flag->flag = 0;
	initialize_env(env_list);
	change_shell_lvl(env_list);
	ft_init_builtins(builtins);
	signal(SIGINT, handle_signal);
	signal(SIGQUIT, handle_signal);
}

int	read_and_tokenize_input(char **input, Token **tokens)
{
	*input = ft_read_input();
	if (!*input)
		return (-1);
	*tokens = tokenize_input(*input);
	if (*tokens == NULL || (*tokens)->type == TOKEN_ERROR)
	{
		free(*input);
		*input = NULL;
		return (0);
	}
	return (1);
}

Command	*parse_commands(Token **tokens, envvar **env_list,
		shell_status *e_status)
{
	Command	*commands;

	commands = parse_pipeline(tokens, env_list, e_status);
	if (!commands)
		e_status->last_exit_status = 1;
	return (commands);
}
