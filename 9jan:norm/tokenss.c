/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenss.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/09 21:00:45 by hawild            #+#    #+#             */
/*   Updated: 2025/01/09 22:02:38 by hawild           ###   ########.fr       */
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

Token *process_pipe(char **word, Token **head, Token **tail)
{
    Token *new_t = NULL;

    if (*word != NULL)
    {
        new_t = new_token(TOKEN_WORD, *word);
        append_token(head, tail, new_t);
        free(*word);
        *word = NULL;
    }
    new_t = new_token(TOKEN_PIPE, "|");
    append_token(head, tail, new_t);

    return new_t;
}
Token *process_input_redirect(char **word, Token **head, Token **tail, char *input, int *i)
{
	Token *new_t = NULL;

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
	return new_t;
}
Token *process_output_redirect(char **word, Token **head, Token **tail, char *input, int *i)
{
	Token *new_t = NULL;

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
	return new_t;
}
Token *process_single_quote(char **word, Token **head, Token **tail, char *input, int *i, int *double_quote_count)
{
	Token *new_t = NULL;

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
			return new_token(TOKEN_ERROR, "Unclosed single quote");
		}
		(*i)++;
	}
	else
	{
		append_char(word, input[*i]);
		(*i)++;
	}
	return new_t;
}
Token *process_dollar_sign(char **word, Token **head, Token **tail, char *input, int *i, int *double_quote_count)
{
    Token *new_t = NULL;

    if (input[*i + 1] != '"')
    {
        new_t = handle_dollar(input, i);
        Token *merge_token = NULL;
        if (*word != NULL)
        {
            Token *word_token = new_token(TOKEN_WORD, *word);
            append_token(head, tail, word_token);
            merge_token = new_token(TOKEN_MERGE_FLAG, "YES");
            append_token(head, tail, merge_token);
            free(*word);
            *word = NULL;
        }
        if (new_t != NULL)
        {
            append_token(head, tail, new_t);
            if (*double_quote_count % 2 != 0 && input[*i] != '\0' && isspace(input[*i]))
            {
                Token *xtramerge = new_token(TOKEN_MERGE_FLAG, "YES");
                append_token(head, tail, xtramerge);
            }
            if (input[*i] != '\0' && !isspace(input[*i]))
            {
                merge_token = new_token(TOKEN_MERGE_FLAG, "YES");
                append_token(head, tail, merge_token);
            }
        }
    }
    else
    {
        append_char(word, input[*i]);
        (*i)++;
    }
    return new_t;
}
void process_special_tokens(char *input, int *i, int *double_quote_count, char **word, Token **head, Token **tail)
{
    Token *new_t = NULL;

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
    {
        append_char(word, input[*i]);
        (*i)++;
    }
}


Token	*tokenize_input(char *input)
{
	Token *head = NULL;
	Token *tail = NULL;
	int i = 0;
	int space = 0;
	int double_quote_count = 0;
	char *word = NULL;
    Token *new_t;

	while (input[i] != '\0')
	{
		if (isspace(input[i]))
		{
			if (isspace(input[i]))
			{
				process_whitespace(&word, &head, &tail, &double_quote_count,
					input[i]);
				i++;
				continue ;
			}
		}
		new_t = NULL;
		process_special_tokens(input, &i, &double_quote_count, &word, &head, &tail);
	}
	if (word != NULL)
	{
		Token *new_t = new_token(TOKEN_WORD, word);
		append_token(&head, &tail, new_t);
		free(word);
	}
	if (double_quote_count % 2 != 0)
	{
		return (new_token(TOKEN_ERROR, "Unclosed double quote"));
	}
	return (head);
}