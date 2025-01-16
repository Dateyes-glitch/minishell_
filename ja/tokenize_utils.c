/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 23:45:50 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:47:10 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

Token	*new_token(int type, char *value)
{
	Token	*token;

	token = malloc(sizeof(Token));
	if (!token)
	{
		perror("minishell: malloc");
		exit(EXIT_FAILURE);
	}
	token->type = type;
	token->value = strdup(value);
	token->next = NULL;
	return (token);
}

Token	*handle_dollar(char *input, int *i)
{
	Token	*new_t;
	char	*default_status;
	int		start;
	char	*env_var;

	new_t = NULL;
	(*i)++;
	if (input[*i] == '?')
	{
		(*i)++;
		default_status = strdup("0");
		new_t = new_token(TOKEN_EXIT_STATUS, default_status);
		free(default_status);
	}
	else
	{
		start = *i;
		while (isalnum(input[*i]) || input[*i] == '_')
			(*i)++;
		env_var = strndup(&input[start], *i - start);
		new_t = new_token(TOKEN_ENV_VAR, env_var);
		free(env_var);
	}
	return (new_t);
}

void	append_char(char **str, char c)
{
	int		len;
	char	*new_str;

	if (*str == NULL)
		len = 0;
	else
		len = strlen(*str);
	new_str = (char *)malloc(len + 2);
	if (new_str == NULL)
	{
		perror("minishell: malloc failed");
		exit(1);
	}
	if (*str != NULL)
	{
		strcpy(new_str, *str);
		free(*str);
	}
	new_str[len] = c;
	new_str[len + 1] = '\0';
	*str = new_str;
}

void	append_str(char **dest, const char *src)
{
	int		dest_len;
	int		src_len;
	char	*new_str;

	if (src == NULL)
		return ;
	if (*dest == NULL)
		dest_len = 0;
	else
		dest_len = strlen(*dest);
	src_len = strlen(src);
	new_str = (char *)malloc(dest_len + src_len + 1);
	if (new_str == NULL)
	{
		perror("minishell: malloc failed");
		exit(1);
	}
	if (*dest != NULL)
	{
		strcpy(new_str, *dest);
		free(*dest);
	}
	strcpy(new_str + dest_len, src);
	*dest = new_str;
}

void	append_token(Token **head, Token **tail, Token *new_t)
{
	if (new_t)
	{
		if (*tail)
		{
			(*tail)->next = new_t;
		}
		else
		{
			*head = new_t;
		}
		*tail = new_t;
	}
}
