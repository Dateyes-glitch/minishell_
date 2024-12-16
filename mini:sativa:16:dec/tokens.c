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
//     int flag = 0;

//     while (input[i] != '\0')
//     {
//         if (isspace(input[i]))
//         {
//             i++;
//             continue;
//         }

//         Token *new_t = NULL;

//         if (input[i] == '|')
//         {
//             new_t = new_token(TOKEN_PIPE, "|");
//             i++;
//         }
//         else if (input[i] == '<') 
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
//         else if (input[i] == '>')
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
//         // Handle quoted strings
//         else if (input[i] == '"') 
//         {
//             char quote_char = input[i];
//             i++;
//             int start = i;

//             while (input[i] != '\0' && input[i] != quote_char) 
//             {
//                 while (input[i] == ' ' || input[i] == '\t')
//                     i++;
//                 start = i;
//                 while (input[i] != ' ' && input[i] != '\t' && input[i] != quote_char && input[i] != '\0')
//                     i++;
//                 if (input[i] == '\0')
//                 {
//                     perror("minishell: unclosed quote");
//                     return new_token(TOKEN_ERROR, "Unclosed quote");
//                 }
//                 if (i > start) 
//                 {
//                     char *segment = strndup(&input[start], i - start);
//                     if (segment[0] == '$' && segment[1] != '?' && quote_char == '"') 
//                     {
//                         char *segment = strndup(&input[start + 1], i - start - 1);
//                         new_t = new_token(TOKEN_ENV_VAR, segment);
//                     }
//                     else if (segment[0] == '$' && segment[1] == '?' && quote_char == '"')
//                     {
//                         char *default_status = strdup("0");
//                         new_t = new_token(TOKEN_EXIT_STATUS, default_status);
//                         free(default_status);
//                     }
//                     else 
//                     {
//                         new_t = new_token(TOKEN_WORD, segment);
//                     }
//                     free(segment);
//                     if (new_t) 
//                     {
//                         if (tail) tail->next = new_t;
//                         else head = new_t;
//                         tail = new_t;
//                         flag = 1;
//                     }
//                 }
//             }
//             if (input[i] == quote_char)
//                 i++;
//         }
//         // Single quotes
//         else if (input[i] == '\'')
//         {
//             char quote_char = input[i++];
//             int start = i;
//             while (input[i] != quote_char && input[i] != '\0') i++;
//             if (input[i] == '\0')
//             {
//                 perror("minishell: unclosed quote");
//                 return new_token(TOKEN_ERROR, "Unclosed quote");
//             }
//             char *quote_str = strndup(&input[start], i - start);
//             new_t = new_token(TOKEN_QUOTE, quote_str);
//             free(quote_str);
//             i++;
//         }

//         // Handle environment variables and exit status 
//         else if (input[i] == '$')
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
//             while (!isspace(input[i]) && input[i] != '|' && input[i] != '<' && input[i] != '>' && input[i] != '\0') i++;
//             char *word = strndup(&input[start], i - start);
//             new_t = new_token(TOKEN_WORD, word);
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
//     }

//     return (head);
// }


Token *tokenize_input(char *input)
{
    Token *head = NULL;
    Token *tail = NULL;
    int i = 0;
    int flag = 0;
    Token *xtra = NULL;

    while (input[i] != '\0')
    {
        if (isspace(input[i]))
        {
            i++;
            continue;
        }

        Token *new_t = NULL;

        if (input[i] == '|')
        {
            new_t = new_token(TOKEN_PIPE, "|");
            i++;
        }
        else if (input[i] == '<') 
        {
            if (input[i + 1] == '<') 
            {
                new_t = new_token(TOKEN_HEREDOC, "<<");
                i += 2;
            } 
            else 
            {
                new_t = new_token(TOKEN_INPUT_REDIRECT, "<");
                i++;
            }
        }
        else if (input[i] == '>')
        {
            if (input[i + 1] == '>')
            {
                new_t = new_token(TOKEN_APPEND_REDIRECT, ">>");
                i += 2;
            }
            else
            {
                new_t = new_token(TOKEN_OUTPUT_REDIRECT, ">");
                i++;
            }
        }
        // Handle quoted strings
        else if (input[i] == '"') 
        {
            char quote_char = input[i];
            i++;
            int start = i;

            while (input[i] != '\0' && input[i] != quote_char) 
            {
                while (input[i] == ' ' || input[i] == '\t')
                    i++;
                start = i;
                while (input[i] != ' ' && input[i] != '\t' && input[i] != quote_char && input[i] != '\0')
                    i++;
                if (input[i] == '\0')
                {
                    perror("minishell: unclosed quote");
                    return new_token(TOKEN_ERROR, "Unclosed quote");
                }
                if (i > start) 
                {
                    char *segment = strndup(&input[start], i - start);
                    if (segment[0] == '$' && segment[1] != '?' && quote_char == '"') 
                    {
                        char *segment = strndup(&input[start + 1], i - start - 1);
                        new_t = new_token(TOKEN_ENV_VAR, segment);
                    }
                    else if (segment[0] == '$' && segment[1] == '?' && quote_char == '"')
                    {
                        char *default_status = strdup("0");
                        new_t = new_token(TOKEN_EXIT_STATUS, default_status);
                        free(default_status);
                    }
                    else 
                    {
                        new_t = new_token(TOKEN_WORD, segment);
                    }
                    free(segment);
                    if (new_t) 
                    {
                        if (tail) tail->next = new_t;
                        else head = new_t;
                        tail = new_t;
                        flag = 1;
                    }
                }
            }
            if (input[i] == quote_char)
                i++;
        }
        // Single quotes
        else if (input[i] == '\'')
        {
            char quote_char = input[i++];
            int start = i;
            while (input[i] != quote_char && input[i] != '\0') i++;
            if (input[i] == '\0')
            {
                perror("minishell: unclosed quote");
                return new_token(TOKEN_ERROR, "Unclosed quote");
            }
            char *quote_str = strndup(&input[start], i - start);
            new_t = new_token(TOKEN_QUOTE, quote_str);
            free(quote_str);
            i++;
            if (input[i] == '\'' || (input[i] != ' ' && input[i] != '\t' && input[i] != '\0'))
            {
                xtra = new_token(TOKEN_MERGE_FLAG,"YES");
                //printf("here\n");
            }
        }

        // Handle environment variables and exit status 
        else if (input[i] == '$')
        {
            if (input[++i] == '?')
            {
                i++;
                char *default_status = strdup("0");
                new_t = new_token(TOKEN_EXIT_STATUS, default_status);
                free(default_status);
            }
            else
            {
                int start = i;
                while (isalnum(input[i]) || input[i] == '_') i++;
                char *env_var = strndup(&input[start], i - start);
                new_t = new_token(TOKEN_ENV_VAR, env_var);
                
                free(env_var);
            }
        }
        // Handle normal words
        else
        {
            
            int start = i;
            while (!isspace(input[i]) && input[i] != '|' && input[i] != '<' && input[i] != '>' && input[i] != '\'' && input[i] != '\0') {i++;}
            // if (input[i] == '\'')
            //     xtra = new_token(TOKEN_MERGE_FLAG,"YES");
            char *word = strndup(&input[start], i - start);
            new_t = new_token(TOKEN_WORD, word);
            if (input[i] == '\'')
            {
                xtra = new_token(TOKEN_MERGE_FLAG,"YES");
            }
            free(word);
        }

        // Append the new token to the token list
        if (new_t && flag != 1) 
        {
            if (tail) tail->next = new_t;
            else head = new_t;
            tail = new_t;
        }
        else if (flag == 1)
            flag = 0;
        if (xtra != NULL)
        {
            if (tail) tail->next = xtra;
            else head = xtra;
            tail = xtra;
            xtra = NULL;
        }
    }
    // if (input[i] == '\0' && head == NULL)
    //     head->type = TOKEN_EMPTY;
    return (head);
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
