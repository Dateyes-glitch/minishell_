/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_utils_3.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 23:09:21 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:45:24 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

Token	*process_input_redirect(char **word, Token **head, Token **tail,
		char *input, int *i)
{
	Token	*new_t;

	new_t = NULL;
	if (*word != NULL)
	{
		new_t = new_token(TOKEN_WORD, *word);
		append_token(head, tail, new_t);
		free(*word);
		*word = NULL;
	}
	if (input[*i + 1] == '<')
	{
		new_t = new_token(TOKEN_HEREDOC, "<<");
		append_token(head, tail, new_t);
		*i += 2;
	}
	else
	{
		new_t = new_token(TOKEN_INPUT_REDIRECT, "<");
		append_token(head, tail, new_t);
		(*i)++;
	}
	return (new_t);
}

Token	*process_output_redirect(char **word, Token **head, Token **tail,
		char *input, int *i)
{
	Token	*new_t;

	new_t = NULL;
	if (*word != NULL)
	{
		new_t = new_token(TOKEN_WORD, *word);
		append_token(head, tail, new_t);
		free(*word);
		*word = NULL;
	}
	if (input[*i + 1] == '>')
	{
		new_t = new_token(TOKEN_APPEND_REDIRECT, ">>");
		append_token(head, tail, new_t);
		*i += 2;
	}
	else
	{
		new_t = new_token(TOKEN_OUTPUT_REDIRECT, ">");
		append_token(head, tail, new_t);
		(*i)++;
	}
	return (new_t);
}

Token	*process_single_quote(char **word, Token **head, Token **tail,
		char *input, int *i, int *double_quote_count)
{
	Token	*new_t;

	new_t = NULL;
	if (*double_quote_count % 2 == 0)
	{
		(*i)++;
		while (input[*i] != '\'' && input[*i] != '\0')
		{
			append_char(word, input[*i]);
			(*i)++;
		}
		if (input[*i] == '\0')
		{
			free(*word);
			return (new_token(TOKEN_ERROR, "Unclosed single quote"));
		}
		(*i)++;
	}
	else
	{
		append_char(word, input[*i]);
		(*i)++;
	}
	return (new_t);
}
