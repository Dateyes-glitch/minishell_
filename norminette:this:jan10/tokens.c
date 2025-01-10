# include "minishell.h"

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

	len = (*str == NULL) ? 0 : strlen(*str);
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
	{
		return ;
	}
	dest_len = (*dest == NULL) ? 0 : strlen(*dest);
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

Token *tokenize_input(char *input)
{
    Token *head = NULL;
    Token *tail = NULL;
    int i = 0;
    int space = 0;
    int double_quote_count = 0; 
    char *word = NULL;

    while (input[i] != '\0')
    {
        if (isspace(input[i]))
        {
            if (double_quote_count % 2 != 0)
            {
                append_char(&word, input[i]);
                space = 1;
            }
            else if (word != NULL) {
                Token *new_t = new_token(TOKEN_WORD, word);
                append_token(&head, &tail, new_t);
                free(word);
                word = NULL;
            }

            i++;
            continue;
        }

        Token *new_t = NULL;

        if (input[i] == '|' && double_quote_count % 2 == 0)
        {
            if (word != NULL) {
                Token *new_t = new_token(TOKEN_WORD, word);
                append_token(&head, &tail, new_t);
                free(word);
                word = NULL;
            }
            new_t = new_token(TOKEN_PIPE, "|");
            append_token(&head, &tail, new_t);
            i++;
        }
        else if (input[i] == '<' && double_quote_count % 2 == 0) 
        {
            if (word != NULL) {
                Token *new_t = new_token(TOKEN_WORD, word);
                append_token(&head, &tail, new_t);
                free(word);
                word = NULL;
            }
            if (input[i + 1] == '<') 
            {
                new_t = new_token(TOKEN_HEREDOC, "<<");
                append_token(&head, &tail, new_t);
                i += 2;
            } 
            else 
            {
                new_t = new_token(TOKEN_INPUT_REDIRECT, "<");
                append_token(&head, &tail, new_t);
                i++;
            }
        }
        else if (input[i] == '>' && double_quote_count % 2 == 0)
        {
            if (word != NULL) {
                Token *new_t = new_token(TOKEN_WORD, word);
                append_token(&head, &tail, new_t);
                free(word);
                word = NULL;
            }
            if (input[i + 1] == '>')
            {
                new_t = new_token(TOKEN_APPEND_REDIRECT, ">>");
                append_token(&head, &tail, new_t);
                i += 2;
            }
            else
            {
                new_t = new_token(TOKEN_OUTPUT_REDIRECT, ">");
                append_token(&head, &tail, new_t);
                i++;
            }
        }
        else if (input[i] == '"') 
        {
            double_quote_count++;
            i++; 
        } 
        else if (input[i] == '\'')
        {
            if (double_quote_count % 2 == 0) { 
                i++; 
                while (input[i] != '\'' && input[i] != '\0') {
                    append_char(&word, input[i]);
                    i++;
                }
                if (input[i] == '\0') {
                    free(word);
                    return new_token(TOKEN_ERROR, "Unclosed single quote");
                }
                i++; 
            } else {
                append_char(&word, input[i]); 
                i++;
            }
        }
        else if (input[i] == '$' && input[i + 1] != '\0' && (isalnum(input[i + 1]) || input[i + 1] == '?')) 
        {
            if (input[i + 1] != '"') {
                new_t = handle_dollar(input, &i);
                Token *merge_token = NULL;
                if (word != NULL)
                {
                    Token *word_token = new_token(TOKEN_WORD, word);
                    append_token(&head, &tail, word_token);
                    merge_token = new_token(TOKEN_MERGE_FLAG, "YES");
                    append_token(&head, &tail, merge_token);
                    free(word);
                    word = NULL;
                }
                if (new_t != NULL) {
                    append_token(&head, &tail, new_t);
                    if (double_quote_count % 2 != 0 && input[i] != '\0' && isspace(input[i]))
                    {
                        Token *xtramerge = new_token(TOKEN_MERGE_FLAG, "YES");
                        append_token(&head, &tail, xtramerge);
                    }
                    if (double_quote_count % 2 == 0 && input[i] != '\0' && !isspace(input[i]) && isalnum(input[i]))
                    {
                        merge_token = new_token(TOKEN_MERGE_FLAG, "YES");
                        append_token(&head, &tail, merge_token);
                    }
                    else if (double_quote_count % 2 != 0 && input[i] != '\0' && !isspace(input[i]))
                    {
                        merge_token = new_token(TOKEN_MERGE_FLAG, "YES");
                        append_token(&head, &tail, merge_token);
                    }
                }
            } else {
                append_char(&word, input[i]);
                i++;
                space = 0;
            }
        }
        else 
        {
            append_char(&word, input[i]);
            i++;
        }
    }

    if (word != NULL) {
        Token *new_t = new_token(TOKEN_WORD, word);
        append_token(&head, &tail, new_t);
        free(word);
    }

    if (double_quote_count % 2 != 0) {
        return new_token(TOKEN_ERROR, "Unclosed double quote");
    }

    return head;
}

void	free_tokens(Token *tokens)
{
	Token *current = tokens;
	while (current)
	{
		Token *next = current->next;
		free(current->value);
		free(current);
		current = next;
	}
}
