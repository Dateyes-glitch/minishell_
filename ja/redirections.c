/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 22:51:12 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 22:51:41 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_output_redirection(Command *cmd, int append)
{
	int	fd_out;

	if (append)
		fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_out < 0)
	{
		write(2, "minishell: ", 11);
		write(2, cmd->output_file, strlen(cmd->output_file));
		write(2, ": file not found\n", 17);
		cmd->exit_status = 1;
		exit(EXIT_FAILURE);
	}
	if (dup2(fd_out, STDOUT_FILENO) < 0)
	{
		perror("minishell: dup2 output");
		close(fd_out);
		cmd->exit_status = 1;
		exit(EXIT_FAILURE);
	}
	close(fd_out);
}

static void	handle_input_redirection(Command *cmd)
{
	int	fd_in;

	fd_in = open(cmd->input_file, O_RDONLY);
	if (fd_in < 0)
	{
		write(2, "minishell: ", 11);
		write(2, cmd->input_file, strlen(cmd->input_file));
		write(2, ": input file not found\n", 23);
		exit(cmd->exit_status);
	}
	if (dup2(fd_in, STDIN_FILENO) < 0)
	{
		perror("minishell: dup2 input");
		close(fd_in);
		cmd->exit_status = 1;
		exit(EXIT_FAILURE);
	}
	close(fd_in);
}

static void	handle_redirections_for_output(Command *cmd)
{
	if (cmd->output_file)
		handle_output_redirection(cmd, cmd->append);
}

static void	handle_redirections_for_input(Command *cmd)
{
	if (cmd->input_file)
		handle_input_redirection(cmd);
}

void	handle_redirections(Command *cmd)
{
	if (!cmd || !cmd->args)
		return ;
	if (cmd->output == 1)
	{
		handle_redirections_for_output(cmd);
		handle_redirections_for_input(cmd);
	}
	else
	{
		handle_redirections_for_input(cmd);
		handle_redirections_for_output(cmd);
	}
}
