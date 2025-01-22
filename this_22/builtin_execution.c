/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_execution.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 23:15:02 by hawild            #+#    #+#             */
/*   Updated: 2025/01/22 17:45:07 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_init_builtins(t_builtin_cmd *builtins)
{
	builtins[0].name = ft_strdup("cd");
	builtins[0].func = cmd_cd;
	builtins[1].name = ft_strdup("echo");
	builtins[1].func = cmd_echo;
	builtins[2].name = ft_strdup("pwd");
	builtins[2].func = cmd_pwd;
	builtins[3].name = ft_strdup("export");
	builtins[3].func = cmd_export;
	builtins[4].name = ft_strdup("unset");
	builtins[4].func = cmd_unset;
	builtins[5].name = ft_strdup("env");
	builtins[5].func = cmd_env;
	builtins[6].name = ft_strdup("exit");
	builtins[6].func = cmd_exit;
}

int	ft_execute_builtin(t_command *cmd, t_builtin_cmd *builtins,
		t_envvar **env_list)
{
	int	i;

	if (!cmd->args || !cmd->args[0])
		printf("Error: args is NULL or args[0] is NULL\n");
	i = 0;
	while (i < NUM_BUILTINS)
	{
		if (ft_strcmp(cmd->args[0], builtins[i].name) == 0)
		{
			builtins[i].func(cmd, env_list);
			return (0);
		}
		i++;
	}
	return (1);
}
