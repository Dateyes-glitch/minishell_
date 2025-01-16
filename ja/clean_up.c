/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_up.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 20:55:27 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:20:31 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_tokens(Token *tokens)
{
	Token	*current;
	Token	*next;

	current = tokens;
	while (current)
	{
		next = current->next;
		free(current->value);
		free(current);
		current = next;
	}
}

void	free_commands(Command *commands)
{
	Command	*current;
	Command	*next;
	int		i;

	current = commands;
	while (current)
	{
		next = current->next;
		if (current->args)
		{
			i = 0;
			while (current->args[i])
			{
				free(current->args[i]);
				i++;
			}
			free(current->args);
		}
		if (current->input_file)
			free(current->input_file);
		if (current->output_file)
			free(current->output_file);
		current = next;
	}
}

void	ft_free(envvar **env_list)
{
	envvar	*current;
	envvar	*next_node;

	current = *env_list;
	while (current)
	{
		next_node = current->next;
		free(current->key);
		free(current->value);
		free(current);
		current = next_node;
	}
}

void	cleanup_iteration(char *input, Token *tokens, Command *commands)
{
	free_tokens(tokens);
	free_commands(commands);
	free(input);
}

void	cleanup_shell(envvar **env_list, builtin_cmd_t *builtins)
{
	int	i;

	ft_free(env_list);
	i = 0;
	while (i < NUM_BUILTINS)
	{
		free(builtins[i].name);
		i++;
	}
}
