/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 22:49:49 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:39:23 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			execute_external_running = 0;

void	handle_signal(int sig)
{
	if (sig == SIGINT)
	{
		if (execute_external_running == 1)
		{
			printf("\n");
		}
		else
		{
			printf("\n");
			rl_on_new_line();
			rl_replace_line("", 0);
			rl_redisplay();
		}
		execute_external_running = 130;
	}
	else if (sig == SIGQUIT)
	{
	}
}

static void	update_exit_status(Command *commands, shell_status *e_status)
{
	Command	*current;

	if (execute_external_running == 1)
	{
		e_status->last_exit_status = 130;
		return ;
	}
	if (commands != NULL && commands->next == NULL)
	{
		e_status->last_exit_status = commands->exit_status;
		return ;
	}
	current = commands;
	while (current != NULL && current->exit_status == 0)
		current = current->next;
	if (current == NULL)
		e_status->last_exit_status = 0;
	else
		e_status->last_exit_status = current->exit_status;
}

void	ft_run_shell(envvar *env_list, builtin_cmd_t *builtins,
		Command *commands, Token *tokens)
{
	char			*input;
	int				status;
	shell_status	e_status;
	unset_path_flag	unset_flag;
	int				input_status;

	input = NULL;
	status = 0;
	initialize_shell(&env_list, builtins, &e_status, &unset_flag);
	while (status == 0)
	{
		input_status = read_and_tokenize_input(&input, &tokens);
		if (input_status == -1)
			break ;
		if (input_status == 0)
			continue ;
		commands = parse_commands(&tokens, &env_list, &e_status);
		status = ft_execute_parsed_commands(commands, builtins, &env_list,
				&e_status, &unset_flag);
		update_exit_status(commands, &e_status);
		cleanup_iteration(input, tokens, commands);
	}
	cleanup_shell(&env_list, builtins);
}

int	main(void)
{
	envvar			*env_list;
	Token			*tokens;
	Command			*commands;
	builtin_cmd_t	builtins[NUM_BUILTINS];

	env_list = NULL;
	tokens = NULL;
	commands = NULL;
	ft_run_shell(env_list, builtins, commands, tokens);
	return (0);
}
