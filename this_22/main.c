/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 22:49:49 by hawild            #+#    #+#             */
/*   Updated: 2025/01/22 22:24:41 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			g_execute_external_running;

void	handle_signal(int sig)
{
	if (sig == SIGINT)
	{
		if (g_execute_external_running == 1)
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
		g_execute_external_running = 130;
	}
	else if (sig == SIGQUIT)
	{
	}
}

static void	update_exit_status(t_command *commands, t_shell_status *e_status)
{
	t_command	*current;

	if (g_execute_external_running == 1)
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

// void	ft_run_shell(t_envvar *env_list, t_builtin_cmd *builtins,
// 		t_command *commands, t_token *tokens)
// {
// 	char			*input;
// 	int				status;
// 	t_shell_status	e_status;
// 	int				input_status;

// 	input = NULL;
// 	status = 0;
// 	initialize_shell(&env_list, builtins, &e_status);
// 	while (status == 0)
// 	{
// 		input_status = read_and_tokenize_input(&input, &tokens);
// 		if (input_status == -1)
// 			break ;
// 		if (input_status == 0)
// 			continue ;
// 		commands = parse_commands(&tokens, &env_list, &e_status);
// 		status = ft_execute_parsed_commands(commands, builtins, &env_list,
// 				&e_status);
// 		update_exit_status(commands, &e_status);
// 		cleanup_iteration(input, tokens, commands);
// 	}
// 	cleanup_shell(&env_list, builtins);
// }

void	ft_run_shell(t_envvar *env_list, t_builtin_cmd *builtins,
		t_command *commands, t_token *tokens)
{
	char			*input;
	int				status;
	t_shell_status	e_status;
	int				input_status;

	input = NULL;
	status = 0;
	initialize_shell(&env_list, builtins, &e_status);
	while (status == 0)
	{
		signal(SIGINT, handle_signal);
		signal(SIGQUIT, handle_signal);
		input_status = read_and_tokenize_input(&input, &tokens);
		if (input_status == -1)
		{
			ft_free(&env_list);
    		for (int i = 0; i < NUM_BUILTINS; i++)
        		free(builtins[i].name);
			exit(1);
		}
		if (input_status == 0)
		{
			if (tokens == NULL)
			{
			}
			else if (tokens->type == TOKEN_ERROR)
			{
				e_status.last_exit_status = 1;
				printf("minishell:unclosed quotes\n");
				free_tokens(tokens);
			}
			free(input);
			continue;
		}
		commands = parse_commands(&tokens, &env_list, &e_status);
		if (!commands && e_status.last_exit_status == 0)
		{
			e_status.last_exit_status = 1;
			continue;
		}
		if (commands->exit_status == 2)
		{
			e_status.last_exit_status = 2;
			free_tokens(tokens);
			free_commands(commands);
			continue;
		}
		t_command *point = commands;
		while (point && point->exit_status != 127 && point->exit_status != 300)
			point = point->next;
		if (point != NULL)
		{
			if (point->exit_status == 127)
				printf("minishell: permissions denied: %s\n", point->input_file);
			else if (point->exit_status == 300)
				printf("minishell: permissions denied: %s\n", point->output_file);
			e_status.last_exit_status = 1;
			free_tokens(tokens);
			free_commands(commands);
			continue;

		}
		status = ft_execute_parsed_commands(commands, builtins, &env_list,
				&e_status);
		t_command *another = commands;
        int j = 0;
		while (another && another->next && (another->exit_status!=130 && another->exit_status != 131))
			another = another->next;
		e_status.last_exit_status = another->exit_status;
		// update_exit_status(commands, &e_status);
		cleanup_iteration(input, tokens, commands);
		g_execute_external_running = 0;
	}
	cleanup_shell(&env_list, builtins);
	if (status == 400)
		exit(0);
	if (status == 500)
		exit(2);
	if (status != 0 && status != 1)
		exit(status);
}

int	main(void)
{
	t_envvar		*env_list;
	t_token			*tokens;
	t_command		*commands;
	t_builtin_cmd	builtins[NUM_BUILTINS];

	env_list = NULL;
	tokens = NULL;
	commands = NULL;
	ft_run_shell(env_list, builtins, commands, tokens);
	return (0);
}
