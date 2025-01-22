/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_up.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 20:55:27 by hawild            #+#    #+#             */
/*   Updated: 2025/01/17 17:27:22 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_tokens(t_token *tokens)
{
	t_token	*current;
	t_token	*next;

	current = tokens;
	while (current)
	{
		next = current->next;
		free(current->value);
		free(current);
		current = next;
	}
}

void	free_commands(t_command *commands)
{
	t_command	*current;
	t_command	*next;
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

void	ft_free(t_envvar **env_list)
{
	t_envvar	*current;
	t_envvar	*next_node;

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

void	cleanup_iteration(char *input, t_token *tokens, t_command *commands)
{
	free_tokens(tokens);
	free_commands(commands);
	free(input);
}

void	cleanup_shell(t_envvar **env_list, t_builtin_cmd *builtins)
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
