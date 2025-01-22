/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_utils_3.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 23:09:21 by hawild            #+#    #+#             */
/*   Updated: 2025/01/17 17:49:27 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*process_input_redirect(t_tokenization_vars *tvars, char *input)
{
	t_token	*new_t;

	new_t = NULL;
	if (tvars->word != NULL)
	{
		new_t = new_token(TOKEN_WORD, tvars->word);
		append_token(&tvars->head, &tvars->tail, new_t);
		free(tvars->word);
		tvars->word = NULL;
	}
	if (input[tvars->index + 1] == '<')
	{
		new_t = new_token(TOKEN_HEREDOC, "<<");
		append_token(&tvars->head, &tvars->tail, new_t);
		tvars->index += 2;
	}
	else
	{
		new_t = new_token(TOKEN_INPUT_REDIRECT, "<");
		append_token(&tvars->head, &tvars->tail, new_t);
		(tvars->index)++;
	}
	return (new_t);
}

t_token	*process_output_redirect(t_tokenization_vars *tvars, char *input)
{
	t_token	*new_t;

	new_t = NULL;
	if (tvars->word != NULL)
	{
		new_t = new_token(TOKEN_WORD, tvars->word);
		append_token(&tvars->head, &tvars->tail, new_t);
		free(tvars->word);
		tvars->word = NULL;
	}
	if (input[tvars->index + 1] == '>')
	{
		new_t = new_token(TOKEN_APPEND_REDIRECT, ">>");
		append_token(&tvars->head, &tvars->tail, new_t);
		tvars->index += 2;
	}
	else
	{
		new_t = new_token(TOKEN_OUTPUT_REDIRECT, ">");
		append_token(&tvars->head, &tvars->tail, new_t);
		tvars->index++;
	}
	return (new_t);
}

t_token	*process_single_quote(t_tokenization_vars *tvars, char *input,
		int *double_quote_count)
{
	t_token	*new_t;

	new_t = NULL;
	if (*double_quote_count % 2 == 0)
	{
		tvars->index++;
		while (input[tvars->index] != '\'' && input[tvars->index] != '\0')
		{
			append_char(&tvars->word, input[tvars->index]);
			tvars->index++;
		}
		if (input[tvars->index] == '\0')
		{
			free(tvars->word);
			return (new_token(TOKEN_ERROR, "Unclosed single quote"));
		}
		tvars->index++;
	}
	else
	{
		append_char(&tvars->word, input[tvars->index]);
		tvars->index++;
	}
	return (new_t);
}
