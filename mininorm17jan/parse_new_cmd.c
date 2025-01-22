/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_new_cmd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 23:40:58 by hawild            #+#    #+#             */
/*   Updated: 2025/01/17 18:55:09 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_command	*new_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
	{
		perror("minishell: malloc");
		exit(EXIT_FAILURE);
	}
	cmd->args = NULL;
	cmd->input_file = NULL;
	cmd->output_file = NULL;
	cmd->append = 0;
	cmd->exit_status = 0;
	cmd->next = NULL;
	cmd->heredoc = 0;
	cmd->heredoc_delim = NULL;
	cmd->env_exp = NULL;
	cmd->output = 0;
	return (cmd);
}

void	add_argument(t_command *cmd, char *arg)
{
	int	count;

	count = 0;
	while (cmd->args && cmd->args[count])
		count++;
	cmd->args = realloc(cmd->args, (count + 2) * sizeof(char *));
	if (!cmd->args)
	{
		perror("minishell: realloc");
		exit(EXIT_FAILURE);
	}
	cmd->args[count] = ft_strdup(arg);
	cmd->args[count + 1] = NULL;
}

void	add_arguments(t_command *cmd, char **arguments)
{
	int	count;
	int	i;
	int	start;

	count = 0;
	while (cmd->args && cmd->args[count])
		count++;
	start = count;
	i = 0;
	while (arguments[i])
	{
		i++;
		count++;
	}
	cmd->args = realloc(cmd->args, (count + 1) * sizeof(char *));
	if (!cmd->args)
	{
		perror("minishell: malloc");
		exit(EXIT_FAILURE);
	}
	i = 0;
	while (start != count)
		cmd->args[start++] = ft_strdup(arguments[i++]);
	cmd->args[start] = NULL;
}

void	calculate(t_command *cmd)
{
	int	calc;
	int	fd;

	calc = 1000;
	fd = 0;
	if (cmd->input_file || cmd->output_file)
	{
		calc = check_permissions(cmd->input_file);
		if (calc == 5)
			cmd->exit_status = 127;
		else if (calc != 0)
			cmd->exit_status = 126;
		else
			cmd->exit_status = 0;
	}
}
