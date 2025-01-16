/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 20:01:28 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:14:37 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	handle_newline_option(Command *cmd, int *i)
{
	int	newline;

	newline = 1;
	if (cmd->args[1] && strcmp(cmd->args[1], "-n") == 0)
	{
		newline = 0;
		*i = 2;
	}
	return (newline);
}

int	open_output_file(Command *cmd)
{
	int	fd;

	fd = STDOUT_FILENO;
	if (cmd->output_file != NULL)
	{
		if (cmd->append)
			fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
		{
			perror("minishell: echo");
			cmd->exit_status = 1;
		}
	}
	return (fd);
}

void	write_args_to_output(Command *cmd, int fd, int start_index)
{
	int	i;

	i = start_index;
	while (cmd->args[i] != NULL)
	{
		if (i > start_index)
		{
			write(fd, " ", 1);
		}
		write(fd, cmd->args[i], strlen(cmd->args[i]));
		i++;
	}
}

void	handle_newline_write(int newline, int fd)
{
	if (newline)
		write(fd, "\n", 1);
}

void	echo_input_file(Command *cmd)
{
	int	fd;

	fd = 0;
	if (cmd->input_file != NULL)
		fd = open(cmd->input_file, O_RDONLY);
	if (fd < 0)
	{
		write(2, "minishell: ", 11);
		write(2, cmd->input_file, strlen(cmd->input_file));
		write(2, ": No such file or directory.", 28);
		cmd->exit_status = 1;
	}
}
