/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_utils_2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 23:08:28 by hawild            #+#    #+#             */
/*   Updated: 2025/01/14 23:44:22 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

Token	*process_whitespace(char **word, Token **head, Token **tail,
		int *double_quote_count, char input_char)
{
	Token	*new_t;

	if (*double_quote_count % 2 != 0)
		append_char(word, input_char);
	else if (*word != NULL)
	{
		new_t = new_token(TOKEN_WORD, *word);
		append_token(head, tail, new_t);
		free(*word);
		*word = NULL;
	}
	return (NULL);
}

Token	*process_pipe(char **word, Token **head, Token **tail)
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
	new_t = new_token(TOKEN_PIPE, "|");
	append_token(head, tail, new_t);
	return (new_t);
}

void	handle_word_token(char **word, Token **head, Token **tail)
{
	Token	*word_token;
	Token	*merge_token;

	word_token = new_token(TOKEN_WORD, *word);
	append_token(head, tail, word_token);
	merge_token = new_token(TOKEN_MERGE_FLAG, "YES");
	append_token(head, tail, merge_token);
	free(*word);
	*word = NULL;
}
