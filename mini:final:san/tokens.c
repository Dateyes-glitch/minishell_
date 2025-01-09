# include "minishell.h"

Token *new_token(int type, char *value) 
{
    Token *token = malloc(sizeof(Token));
    if (!token)
    {
        perror("minishell: malloc");
        exit(EXIT_FAILURE);
    }
    token->type = type;
    token->value = strdup(value);
    token->next = NULL;
    return token;
}

Token *handle_dollar(char *input, int *i) {
    Token *new_t = NULL;
    (*i)++; // Move past the $

    if (input[*i] == '?') {
        (*i)++;
        char *default_status = strdup("0"); // Or get the actual exit status
        new_t = new_token(TOKEN_EXIT_STATUS, default_status);
        free(default_status);
    } else {
        int start = *i;
        while (isalnum(input[*i]) || input[*i] == '_') {
            (*i)++;
        }
        char *env_var = strndup(&input[start], *i - start);
        new_t = new_token(TOKEN_ENV_VAR, env_var);
        free(env_var);
    }

    return new_t;
}


void append_char(char **str, char c) {
    int len = (*str == NULL) ? 0 : strlen(*str);
    char *new_str = (char *)malloc(len + 2); // +2 for the new char and \0
    if (new_str == NULL) {
        perror("minishell: malloc failed");
        exit(1); 
    }
    if (*str != NULL) {
        strcpy(new_str, *str);
        free(*str);
    }
    new_str[len] = c;
    new_str[len + 1] = '\0';
    *str = new_str;
}

void append_str(char **dest, const char *src) {
    if (src == NULL) {
        return; // Nothing to append
    }
    int dest_len = (*dest == NULL) ? 0 : strlen(*dest);
    int src_len = strlen(src);
    char *new_str = (char *)malloc(dest_len + src_len + 1); // +1 for \0
    if (new_str == NULL) {
        perror("minishell: malloc failed");
        exit(1); 
    }
    if (*dest != NULL) {
        strcpy(new_str, *dest);
        free(*dest);
    }
    strcpy(new_str + dest_len, src);
    *dest = new_str;
}

// Helper function to append tokens to the list
void append_token(Token **head, Token **tail, Token *new_t) {
    if (new_t) {
        if (*tail) {
            (*tail)->next = new_t;
        } else {
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

        if (input[i] == '|' && double_quote_count % 2 == 0) // Check for outside double quotes
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
        else if (input[i] == '$' && input[i + 1] != '\0') 
        {
            if (input[i + 1] != '"') { // Only expand if the next character is not a double quote
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
                    // if (space == 1)
                    // {
                    //     //printf("hereeee");
                    //     Token *xtra_merg = new_token(TOKEN_MERGE_FLAG, "YES");
                    //     append_token(&head, &tail, xtra_merg);
                    // }
                    append_token(&head, &tail, new_t);
                    printf("dqc: %i\n", double_quote_count);
                    printf("i: %c\n", input[i]);
                    if (input[i] != '\0' && !isspace(input[i]))
                    {
                        merge_token = new_token(TOKEN_MERGE_FLAG, "YES");
                        append_token(&head, &tail, merge_token);
                    }
                }
            } else {
                append_char(&word, input[i]); // Include $ in the word if followed by a double quote
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

void free_tokens(Token *tokens) 
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
