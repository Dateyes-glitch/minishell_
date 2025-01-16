/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_input.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 21:00:45 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:43:19 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_new_token(Token *new_t, char *input, int *i,
		int *double_quote_count, Token **head, Token **tail)
{
	Token	*xtramerge;
	Token	*merge_token;

	append_token(head, tail, new_t);
	if (*double_quote_count % 2 != 0 && input[*i] != '\0' && isspace(input[*i]))
	{
		xtramerge = new_token(TOKEN_MERGE_FLAG, "YES");
		append_token(head, tail, xtramerge);
	}
	if (input[*i] != '\0' && !isspace(input[*i]))
	{
		merge_token = new_token(TOKEN_MERGE_FLAG, "YES");
		append_token(head, tail, merge_token);
	}
}

Token	*process_dollar_sign(char **word, Token **head, Token **tail,
		char *input, int *i, int *double_quote_count)
{
	Token	*new_t;
	Token	*merge_token;

	new_t = NULL;
	merge_token = NULL;
	if (input[*i + 1] != '"')
	{
		new_t = handle_dollar(input, i);
		merge_token = NULL;
		if (*word != NULL)
			handle_word_token(word, head, tail);
		if (new_t != NULL)
			handle_new_token(new_t, input, i, double_quote_count, head, tail);
	}
	else
	{
		append_char(word, input[*i]);
		(*i)++;
	}
	return (new_t);
}

void	process_special_tokens(char *input, int *i, int *double_quote_count,
		char **word, Token **head, Token **tail)
{
	Token	*new_t;

	new_t = NULL;
	if (input[*i] == '|' && *double_quote_count % 2 == 0)
	{
		process_pipe(word, head, tail);
		(*i)++;
	}
	else if (input[*i] == '<' && *double_quote_count % 2 == 0)
		process_input_redirect(word, head, tail, input, i);
	else if (input[*i] == '>' && *double_quote_count % 2 == 0)
		process_output_redirect(word, head, tail, input, i);
	else if (input[*i] == '"')
	{
		(*double_quote_count)++;
		(*i)++;
	}
	else if (input[*i] == '\'')
		process_single_quote(word, head, tail, input, i, double_quote_count);
	else if (input[*i] == '$' && input[*i + 1] != '\0')
		process_dollar_sign(word, head, tail, input, i, double_quote_count);
	else
		append_char(word, input[(*i)++]);
}

void	process_input(char *input, int *i, char **word, Token **head,
		Token **tail, int *double_quote_count)
{
	Token	*new_t;

	new_t = NULL;
	if (isspace(input[*i]))
	{
		process_whitespace(word, head, tail, double_quote_count, input[*i]);
		(*i)++;
		return ;
	}
	process_special_tokens(input, i, double_quote_count, word, head, tail);
}

Token	*tokenize_input(char *input)
{
	Token	*head;
	Token	*tail;
	int		i;
	int		double_quote_count;
	char	*word;
	Token	*new_t;

	head = NULL;
	tail = NULL;
	i = 0;
	double_quote_count = 0;
	word = NULL;
	new_t = NULL;
	while (input[i] != '\0')
		process_input(input, &i, &word, &head, &tail, &double_quote_count);
	if (word != NULL)
	{
		new_t = new_token(TOKEN_WORD, word);
		append_token(&head, &tail, new_t);
		free(word);
	}
	if (double_quote_count % 2 != 0)
		return (new_token(TOKEN_ERROR, "Unclosed double quote"));
	return (head);
}
