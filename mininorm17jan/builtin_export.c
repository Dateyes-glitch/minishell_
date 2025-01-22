/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 19:59:38 by hawild            #+#    #+#             */
/*   Updated: 2025/01/17 17:25:35 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	report_invalid_identifier(t_command *cmd, char *arg)
{
	write(STDERR_FILENO, "minishell: export: `", 20);
	write(STDERR_FILENO, arg, strlen(arg));
	write(STDERR_FILENO, "': not a valid identifier\n", 26);
	cmd->exit_status = 1;
}

static void	handle_export_without_value(t_command *cmd, t_envvar **env_list,
		char *arg)
{
	if (is_valid(arg))
		add_or_update_var(env_list, arg, "");
	else
		report_invalid_identifier(cmd, arg);
}

static void	handle_export_with_value(t_command *cmd, t_envvar **env_list, char *arg,
		char *equal_sign)
{
	char	*key;
	char	*value;

	*equal_sign = '\0';
	key = arg;
	value = equal_sign + 1;
	if (is_valid(key))
		add_or_update_var(env_list, key, value);
	else
		report_invalid_identifier(cmd, key);
	*equal_sign = '=';
}

static void	process_export_argument(t_command *cmd, t_envvar **env_list, char *arg)
{
	char	*equal_sign;

	equal_sign = strchr(arg, '=');
	if (equal_sign == NULL)
		handle_export_without_value(cmd, env_list, arg);
	else
		handle_export_with_value(cmd, env_list, arg, equal_sign);
}

int	cmd_export(t_command *cmd, t_envvar **env_list)
{
	int	i;

	if (cmd->args[1] == NULL)
	{
		display_export_vars(env_list);
		cmd->exit_status = 0;
		return (0);
	}
	i = 0;
	while (cmd->args[i] != NULL)
	{
		process_export_argument(cmd, env_list, cmd->args[i]);
		i++;
	}
	return (0);
}
