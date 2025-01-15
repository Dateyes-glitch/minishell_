/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 19:52:33 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:49:04 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_strdup(const char *s)
{
	char	*d;

	d = malloc(strlen(s) + 1);
	if (d)
		strcpy(d, s);
	return (d);
}

int	is_string(char *str)
{
	char	*cpy;
	int		i;
	int		j;

	cpy = strdup(str);
	i = 0;
	j = 0;
	while (cpy[i])
	{
		if (isalpha(cpy[i]))
			j++;
		i++;
	}
	free(cpy);
	if (j != 0)
		return (j);
	return (0);
}

int	is_valid(char *inp)
{
	char	*cpy;
	int		i;
	int		j;

	cpy = strdup(inp);
	i = 0;
	j = 0;
	while (cpy[j] >= '0' && cpy[j] <= '9')
		j++;
	if (cpy[j] == '\0' || cpy[j] == '=')
		return (0);
	while (cpy[i] != '\0' && cpy[i] != '=')
	{
		if (isalnum(cpy[i]) != 0)
			i++;
		else
			cpy[i] = '\0';
	}
	if (i == strlen(inp) || cpy[i] == '=')
		return (1);
	else
		return (0);
}

int	ft_error(Command *cmd, const char *str)
{
	perror(str);
	cmd->exit_status = 1;
	return (1);
}

char	*ft_read_input(void)
{
	char	*input;

	input = readline("minishell>");
	if (input && *input)
		add_history(input);
	return (input);
}
