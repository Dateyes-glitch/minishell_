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

// Token *tokenize_input(char *input)
// {
//     Token *head = NULL;
//     Token *tail = NULL;
//     int i = 0;
//     int mod = 0;
//     int flag = 0;
//     Token *xtra = NULL;

//     while (input[i] != '\0')
//     {
//         if (isspace(input[i]))
//         {
//             i++;
//             continue;
//         }

//         Token *new_t = NULL;

//         if (input[i] == '|' && mod == 0)
//         {
//             new_t = new_token(TOKEN_PIPE, "|");
//             i++;
//         }
//         else if (input[i] == '<' && mod == 0) 
//         {
//             if (input[i + 1] == '<') 
//             {
//                 new_t = new_token(TOKEN_HEREDOC, "<<");
//                 i += 2;
//             } 
//             else 
//             {
//                 new_t = new_token(TOKEN_INPUT_REDIRECT, "<");
//                 i++;
//             }
//         }
//         else if (input[i] == '>' && mod == 0)
//         {
//             if (input[i + 1] == '>')
//             {
//                 new_t = new_token(TOKEN_APPEND_REDIRECT, ">>");
//                 i += 2;
//             }
//             else
//             {
//                 new_t = new_token(TOKEN_OUTPUT_REDIRECT, ">");
//                 i++;
//             }
//         }
//         else if (input[i] == '"')
//         {
//             if (mod == 1)
//             {
//                 mod = 0;
//                 //new_t = new_token(TOKEN_DOUBLE_MERGE, "YES");
//                 i++;
//             }
//             else
//             {
//                 i++;
//                 char *cpy = &input[i];
//                 int j = 0;
//                 while(cpy[j] != '"' && cpy[j] != '\0' && cpy[j] != '$')
//                     j++;
//                 if (cpy[j] == '\0')
//                     return new_t = new_token(TOKEN_ERROR, "Unclosed double quote");
//                 cpy = NULL;
//                 cpy = (char *)malloc((j + 1) * sizeof(char));
//                 j = 0;
//                 while (input[i] != '"' && input[i] != '$')
//                 {
//                     cpy[j] = input[i];
//                     j++;
//                     i++;
//                 }
//                 cpy[j] = '\0';
//                 if (strlen(cpy) != 0)
//                     new_t = new_token(TOKEN_WORD,strdup(cpy));
//                 free(cpy);
//                 mod = 1;

//                 if (input[i] == '$')
//                 {
//                      mod = 1;
//                      //i++;
//                 }
//                 else
//                 {
//                      xtra = new_token(TOKEN_DOUBLE_MERGE, "YES");
//                      //i++;
//                 }
//             }
//         }
//         // Single quotes
//         else if (input[i] == '\'')
//         {
//             char quote_char = input[i++];
//             int start = i;
//             while (input[i] != quote_char && input[i] != '\0') i++;
//             if (input[i] == '\0' && strchr(input, '"') == NULL)
//             {
//                 perror("minishell: unclosed quote");
//                 return new_token(TOKEN_ERROR, "Unclosed quote");
//             }
//             char *quote_str = strndup(&input[start], i - start);
//             new_t = new_token(TOKEN_QUOTE, quote_str);
//             free(quote_str);
//             i++;
//             if (input[i] == '\'' || (input[i] != ' ' && input[i] != '\t' && input[i] != '\0'))
//             {
//                 xtra = new_token(TOKEN_MERGE_FLAG,"YES");
//                 //printf("here\n");
//             }
//         }

//         // Handle environment variables and exit status 
//         else if (input[i] == '$' && input[i + 1] != '\0' && input[i + 1] != '"')
//         {
//             if (input[++i] == '?')
//             {
//                 i++;
//                 char *default_status = strdup("0");
//                 new_t = new_token(TOKEN_EXIT_STATUS, default_status);
//                 free(default_status);
//             }
//             else
//             {
//                 int start = i;
//                 while (isalnum(input[i]) || input[i] == '_') i++;
//                 char *env_var = strndup(&input[start], i - start);
//                 new_t = new_token(TOKEN_ENV_VAR, env_var); 
//                 free(env_var);
//             }
//         }
//         // Handle normal words
//         else
//         {
            
//             int start = i;
//             if (mod == 1 && input[i + 1] != '\0' && input[i + 1] != '"')
//                 while (!isspace(input[i]) && input[i] != '\0' && input[i] != '$'){i++;}
//             else
//                 while (!isspace(input[i]) && input[i] != '|' && input[i] != '<' && input[i] != '>' && input[i] != '\'' && input[i] != '\0' && input[i] != '"') {i++;}
//             // if (input[i] == '\'')
//             //     xtra = new_token(TOKEN_MERGE_FLAG,"YES");
//             char *word = strndup(&input[start], i - start);
//             new_t = new_token(TOKEN_WORD, word);
//             if (input[i] == '\'')
//             {
//                 xtra = new_token(TOKEN_MERGE_FLAG,"YES");
//             }
//             if (input[i] == '"')
//                 xtra = new_token(TOKEN_DOUBLE_MERGE,"YES");
//             free(word);
//         }

//         // Append the new token to the token list
//         if (new_t && flag != 1) 
//         {
//             if (tail) tail->next = new_t;
//             else head = new_t;
//             tail = new_t;
//         }
//         else if (flag == 1)
//             flag = 0;
//         if (xtra != NULL)
//         {
//             if (tail) tail->next = xtra;
//             else head = xtra;
//             tail = xtra;
//             xtra = NULL;
//         }
//     }
//     // if (input[i] == '\0' && head == NULL)
//     //     head->type = TOKEN_EMPTY;
//     return (head);
// }

// Helper function to handle $ and $?
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


// Helper function to append a character to a string
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

// Helper function to append a string to another string
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
    int double_quote_count = 0; 
    char *word = NULL;

    while (input[i] != '\0')
    {
        if (isspace(input[i]))
        {
            if (double_quote_count % 2 != 0)
            {
                append_char(&word, input[i]);
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
                if (word != NULL)
                {
                    Token *word_token = new_token(TOKEN_WORD, word);
                    append_token(&head, &tail, word_token);
                    free(word);
                    word = NULL;
                }
                if (new_t != NULL) {
                    append_token(&head, &tail, new_t);
                }
            } else {
                append_char(&word, input[i]); // Include $ in the word if followed by a double quote
                i++;
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

// Token *tokenize_input(char *input)
// {
//     Token *head = NULL;
//     Token *tail = NULL;
//     int i = 0;
//     int double_quote_count = 0; 
//     char *word = NULL;

//     while (input[i] != '\0')
//     {
//         if (isspace(input[i]))
//         {
//             if (double_quote_count % 2 != 0)
//             {
//                 append_char(&word, input[i]);
//             }
//             else if (word != NULL) {
//                 Token *new_t = new_token(TOKEN_WORD, word);
//                 append_token(&head, &tail, new_t);
//                 free(word);
//                 word = NULL;
//             }

//             i++;
//             continue;
//         }

//         Token *new_t = NULL;

//         if (input[i] == '|' )
//         {
//             new_t = new_token(TOKEN_PIPE, "|");
//             append_token(&head, &tail, new_t);
//             i++;
//         }
//         else if (input[i] == '<' ) 
//         {
//             if (input[i + 1] == '<') 
//             {
//                 new_t = new_token(TOKEN_HEREDOC, "<<");
//                 append_token(&head, &tail, new_t);
//                 i += 2;
//             } 
//             else 
//             {
//                 new_t = new_token(TOKEN_INPUT_REDIRECT, "<");
//                 append_token(&head, &tail, new_t);
//                 i++;
//             }
//         }
//         else if (input[i] == '>' )
//         {
//             if (input[i + 1] == '>')
//             {
//                 new_t = new_token(TOKEN_APPEND_REDIRECT, ">>");
//                 append_token(&head, &tail, new_t);
//                 i += 2;
//             }
//             else
//             {
//                 new_t = new_token(TOKEN_OUTPUT_REDIRECT, ">");
//                 append_token(&head, &tail, new_t);
//                 i++;
//             }
//         }
//         else if (input[i] == '"') 
//         {
//             double_quote_count++;
//             //append_char(&word, input[i]); // Include the double quote in the word
//             i++; 
//         } 
//         else if (input[i] == '\'')
//         {
//             if (double_quote_count % 2 == 0) { // Only process single quotes outside double quotes
//                 i++; // Move past the initial quote
//                 while (input[i] != '\'' && input[i] != '\0') {
//                     append_char(&word, input[i]);
//                     i++;
//                 }
//                 if (input[i] == '\0') {
//                     free(word);
//                     return new_token(TOKEN_ERROR, "Unclosed single quote");
//                 }
//                 i++; // Move past the closing quote
//             } else {

//                 append_char(&word, input[i]); // Include the single quote in the word
//                 i++;
//             }
//         }
//         else if (input[i] == '$' && input[i + 1] != '\0' && input[i + 1] != '"')
//         {
//             new_t = handle_dollar(input, &i);
//             if (new_t != NULL) {
//                 if (word != NULL)
//                 {
//                     Token *word_token = new_token(TOKEN_WORD, word);
//                     append_token(&head, &tail, word_token);
//                     free(word);
//                     word = NULL;
//                 }
//                 append_token(&head, &tail, new_t);
//             }
//         }
//         else 
//         {
//             append_char(&word, input[i]);
//             i++;
//         }
//     }

//     if (word != NULL) {
//         Token *new_t = new_token(TOKEN_WORD, word);
//         append_token(&head, &tail, new_t);
//         free(word);
//     }

//     if (double_quote_count % 2 != 0) {
//         return new_token(TOKEN_ERROR, "Unclosed double quote");
//     }

//     return head;
// }

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
