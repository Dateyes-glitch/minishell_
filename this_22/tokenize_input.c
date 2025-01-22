/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_input.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 21:00:45 by hawild            #+#    #+#             */
/*   Updated: 2025/01/22 20:21:50 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_new_token(t_token *new_t, char *input, int *double_quote_count,
		t_tokenization_vars *tvars)
{
	t_token	*xtramerge;
	t_token	*merge_token;

	append_token(&tvars->head, &tvars->tail, new_t);
	if (*double_quote_count % 2 != 0 && input[tvars->index] != '\0'
		&& ft_isspace(input[tvars->index]))
	{
		xtramerge = new_token(TOKEN_MERGE_FLAG, "YES");
		append_token(&tvars->head, &tvars->tail, xtramerge);
	}
	if (input[tvars->index] != '\0' && !ft_isspace(input[tvars->index]))
	{
		merge_token = new_token(TOKEN_MERGE_FLAG, "YES");
		append_token(&tvars->head, &tvars->tail, merge_token);
	}
}

t_token	*process_dollar_sign(t_tokenization_vars *tvars, char *input,
		int *double_quote_count)
{
	t_token	*new_t;
	t_token	*merge_token;

	new_t = NULL;
	merge_token = NULL;
	if (input[tvars->index + 1] != '"')
	{
		new_t = handle_dollar(input, &tvars->index);
		merge_token = NULL;
		if (tvars->word != NULL)
			handle_word_token(&tvars->word, &tvars->head, &tvars->tail);
		if (new_t != NULL)
			handle_new_token(new_t, input, double_quote_count, tvars);
	}
	else
	{
		append_char(&tvars->word, input[tvars->index]);
		tvars->index++;
	}
	return (new_t);
}

void	process_special_tokens(char *input, int *double_quote_count,
		t_tokenization_vars *tvars)
{
	t_token	*new_t;

	new_t = NULL;
	if (input[tvars->index] == '|' && *double_quote_count % 2 == 0)
	{
		process_pipe(&tvars->word, &tvars->head, &tvars->tail);
		tvars->index++;
	}
	else if (input[tvars->index] == '<' && *double_quote_count % 2 == 0)
		process_input_redirect(tvars, input);
	else if (input[tvars->index] == '>' && *double_quote_count % 2 == 0)
		process_output_redirect(tvars, input);
	else if (input[tvars->index] == '"')
	{
		(*double_quote_count)++;
		tvars->index++;
	}
	else if (input[tvars->index] == '\'')
		process_single_quote(tvars, input, double_quote_count);
	else if (input[tvars->index] == '$' && input[tvars->index + 1] != '\0'
		&& (ft_isalnum(input[tvars->index + 1]) || input[tvars->index
				+ 1] == '?'))
		process_dollar_sign(tvars, input, double_quote_count);
	else
		append_char(&tvars->word, input[tvars->index++]);
}

void	process_input(char *input, t_tokenization_vars *tvars,
		int *double_quote_count)
{
	t_token	*new_t;

	new_t = NULL;
	if (ft_isspace(input[tvars->index]))
	{
		process_whitespace(tvars, double_quote_count, input[tvars->index]);
		tvars->index++;
		return ;
	}
	process_special_tokens(input, double_quote_count, tvars);
}

t_token	*tokenize_input(char *input)
{
	int					double_quote_count;
	t_token				*new_t;
	t_tokenization_vars	tvars;

	double_quote_count = 0;
	new_t = NULL;
	tvars.head = NULL;
	tvars.tail = NULL;
	tvars.index = 0;
	tvars.word = NULL;
	while (input[tvars.index] != '\0')
		process_input(input, &tvars, &double_quote_count);
	if (tvars.word != NULL)
	{
		new_t = new_token(TOKEN_WORD, tvars.word);
		append_token(&tvars.head, &tvars.tail, new_t);
		free(tvars.word);
	}
	if (double_quote_count % 2 != 0)
		return (new_token(TOKEN_ERROR, "Unclosed double quote"));
	return (tvars.head);
}
