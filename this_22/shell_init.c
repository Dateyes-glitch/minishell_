/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 23:00:28 by hawild            #+#    #+#             */
/*   Updated: 2025/01/17 17:41:14 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	initialize_shell(t_envvar **env_list, t_builtin_cmd *builtins,
		t_shell_status *e_status)
{
	e_status->last_exit_status = 0;
	initialize_env(env_list);
	change_shell_lvl(env_list);
	ft_init_builtins(builtins);
	signal(SIGINT, handle_signal);
	signal(SIGQUIT, handle_signal);
}

int	read_and_tokenize_input(char **input, t_token **tokens)
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

t_command	*parse_commands(t_token **tokens, t_envvar **env_list,
		t_shell_status *e_status)
{
	t_command	*commands;

	commands = parse_pipeline(tokens, env_list, e_status);
	if (!commands)
		e_status->last_exit_status = 1;
	return (commands);
}
