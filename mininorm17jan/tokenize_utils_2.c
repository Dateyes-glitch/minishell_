/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_utils_2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 23:08:28 by hawild            #+#    #+#             */
/*   Updated: 2025/01/17 17:42:50 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*process_whitespace(t_tokenization_vars *tvars, int *double_quote_count,
		char input_char)
{
	t_token	*new_t;

	if (*double_quote_count % 2 != 0)
		append_char(&tvars->word, input_char);
	else if (tvars->word != NULL)
	{
		new_t = new_token(TOKEN_WORD, tvars->word);
		append_token(&tvars->head, &tvars->tail, new_t);
		free(tvars->word);
		tvars->word = NULL;
	}
	return (NULL);
}

t_token	*process_pipe(char **word, t_token **head, t_token **tail)
{
	t_token	*new_t;

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

void	handle_word_token(char **word, t_token **head, t_token **tail)
{
	t_token	*word_token;
	t_token	*merge_token;

	word_token = new_token(TOKEN_WORD, *word);
	append_token(head, tail, word_token);
	merge_token = new_token(TOKEN_MERGE_FLAG, "YES");
	append_token(head, tail, merge_token);
	free(*word);
	*word = NULL;
}
