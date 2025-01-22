/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_new_cmd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 23:40:58 by hawild            #+#    #+#             */
/*   Updated: 2025/01/22 17:38:45 by hawild           ###   ########.fr       */
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
	int		count;
	char	**new_args;
	int		i;

	count = 0;
	while (cmd->args && cmd->args[count])
		count++;
	new_args = malloc((count + 2) * sizeof(char *));
	if (!new_args)
	{
		perror("minishell: malloc");
		exit(EXIT_FAILURE);
	}
	i = 0;
	while (i < count)
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	free(cmd->args);
	new_args[count] = ft_strdup(arg);
	new_args[count + 1] = NULL;
	cmd->args = new_args;
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
