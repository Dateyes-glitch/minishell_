/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils_2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 22:58:19 by hawild            #+#    #+#             */
/*   Updated: 2025/01/22 20:03:04 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_permissions(char *filename)
{
	if (access(filename, F_OK) != 0)
		return (3);
	if (access(filename, R_OK) != 0)
		return (1);
	if (access(filename, W_OK) != 0)
		return (2);
	return (0);
}

void	create_unique_temp_file(char *temp_file, int *fd)
{
	int		counter;
	char	*counter_str;

	counter = 0;
	while (1)
	{
		ft_strcpy(temp_file, "/tmp/heredoc_");
		counter_str = ft_itoa(counter);
		ft_strcat(temp_file, counter_str);
		*fd = open(temp_file, O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0644);
		if (*fd != -1)
			break ;
		if (errno != EEXIST)
		{
			perror("minishell: creating temporary file for heredoc");
			return ;
		}
		counter++;
	}
}
