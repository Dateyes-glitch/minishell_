// # include "minishell.h"

// Token *new_token(int type, char *value) 
// {
//     Token *token = malloc(sizeof(Token));
//     if (!token)
//     {
//         perror("minishell: malloc");
//         exit(EXIT_FAILURE);
//     }
//     token->type = type;
//     token->value = strdup(value);
//     token->next = NULL;
//     return token;
// }

// // Token *tokenize_input(char *input)
// // {
// //     Token *head = NULL;
// //     Token *tail = NULL;
// //     int i = 0;
// //     int flag = 0;

// //     while (input[i] != '\0')
// //     {
// //         if (isspace(input[i]))
// //         {
// //             i++;
// //             continue;
// //         }

// //         Token *new_t = NULL;

// //         if (input[i] == '|')
// //         {
// //             new_t = new_token(TOKEN_PIPE, "|");
// //             i++;
// //         }
// //         else if (input[i] == '<') 
// //         {
// //             if (input[i + 1] == '<') 
// //             {
// //                 new_t = new_token(TOKEN_HEREDOC, "<<");
// //                 i += 2;
// //             } 
// //             else 
// //             {
// //                 new_t = new_token(TOKEN_INPUT_REDIRECT, "<");
// //                 i++;
// //             }
// //         }
// //         else if (input[i] == '>')
// //         {
// //             if (input[i + 1] == '>')
// //             {
// //                 new_t = new_token(TOKEN_APPEND_REDIRECT, ">>");
// //                 i += 2;
// //             }
// //             else
// //             {
// //                 new_t = new_token(TOKEN_OUTPUT_REDIRECT, ">");
// //                 i++;
// //             }
// //         }
// //         // Handle quoted strings
// //         else if (input[i] == '"') 
// //         {
// //             char quote_char = input[i];
// //             i++;
// //             int start = i;

// //             while (input[i] != '\0' && input[i] != quote_char) 
// //             {
// //                 while (input[i] == ' ' || input[i] == '\t')
// //                     i++;
// //                 start = i;
// //                 while (input[i] != ' ' && input[i] != '\t' && input[i] != quote_char && input[i] != '\0')
// //                     i++;
// //                 if (input[i] == '\0')
// //                 {
// //                     perror("minishell: unclosed quote");
// //                     return new_token(TOKEN_ERROR, "Unclosed quote");
// //                 }
// //                 if (i > start) 
// //                 {
// //                     char *segment = strndup(&input[start], i - start);
// //                     if (segment[0] == '$' && segment[1] != '?' && quote_char == '"') 
// //                     {
// //                         char *segment = strndup(&input[start + 1], i - start - 1);
// //                         new_t = new_token(TOKEN_ENV_VAR, segment);
// //                     }
// //                     else if (segment[0] == '$' && segment[1] == '?' && quote_char == '"')
// //                     {
// //                         char *default_status = strdup("0");
// //                         new_t = new_token(TOKEN_EXIT_STATUS, default_status);
// //                         free(default_status);
// //                     }
// //                     else 
// //                     {
// //                         new_t = new_token(TOKEN_WORD, segment);
// //                     }
// //                     free(segment);
// //                     if (new_t) 
// //                     {
// //                         if (tail) tail->next = new_t;
// //                         else head = new_t;
// //                         tail = new_t;
// //                         flag = 1;
// //                     }
// //                 }
// //             }
// //             if (input[i] == quote_char)
// //                 i++;
// //         }
// //         // Single quotes
// //         else if (input[i] == '\'')
// //         {
// //             char quote_char = input[i++];
// //             int start = i;
// //             while (input[i] != quote_char && input[i] != '\0') i++;
// //             if (input[i] == '\0')
// //             {
// //                 perror("minishell: unclosed quote");
// //                 return new_token(TOKEN_ERROR, "Unclosed quote");
// //             }
// //             char *quote_str = strndup(&input[start], i - start);
// //             new_t = new_token(TOKEN_QUOTE, quote_str);
// //             free(quote_str);
// //             i++;
// //         }

// //         // Handle environment variables and exit status 
// //         else if (input[i] == '$')
// //         {
// //             if (input[++i] == '?')
// //             {
// //                 i++;
// //                 char *default_status = strdup("0");
// //                 new_t = new_token(TOKEN_EXIT_STATUS, default_status);
// //                 free(default_status);
// //             }
// //             else
// //             {
// //                 int start = i;
// //                 while (isalnum(input[i]) || input[i] == '_') i++;
// //                 char *env_var = strndup(&input[start], i - start);
// //                 new_t = new_token(TOKEN_ENV_VAR, env_var);
                
// //                 free(env_var);
// //             }
// //         }
// //         // Handle normal words
// //         else
// //         {
// //             int start = i;
// //             while (!isspace(input[i]) && input[i] != '|' && input[i] != '<' && input[i] != '>' && input[i] != '\0') i++;
// //             char *word = strndup(&input[start], i - start);
// //             new_t = new_token(TOKEN_WORD, word);
// //             free(word);
// //         }

// //         // Append the new token to the token list
// //         if (new_t && flag != 1) 
// //         {
// //             if (tail) tail->next = new_t;
// //             else head = new_t;
// //             tail = new_t;
// //         }
// //         else if (flag == 1)
// //             flag = 0;
// //     }

// //     return (head);
// // }



// int handle_quotes(char *inp)
// {
//     int j = 0;
//     int i = 0;
//     char *another = strdup(inp);
//     while(another[i])
//     {
//         if (another[i] == '"')
//             j++;
//         i++;
//     }
//     free(another);
//     return j;
// }

// char *handle_double(char *inp)
// {
//     static int j = 0;
//     int i = 0;
//     // char *ret = strdup(inp);
//     if (j == 0)
//     {
//         j = handle_quotes(inp);
//         if (j % 2 != 0)
//             return NULL;
//     }
//     // while(inp[i] && inp[i] == '"')
//     while(inp[i] == '"')
//         i++;
//     return &inp[i];
// }

// char *ft_word(char *str, char *extra) 
// {
//     int j = 0;
//     int extra_len = extra ? strlen(extra) : 0;

//     // Find length of the segment until a double quote or null terminator
//     while (str[j] != '"' && str[j] != '\0')
//         j++;

//     // Allocate or reallocate memory to hold the result
//     char *ret = realloc(extra, extra_len + j + 1);
//     if (!ret) 
//     {
//         perror("minishell: realloc");
//         free(extra);  // Free previous allocation to avoid memory leak
//         return NULL;
//     }

//     // Copy the new part of the string into the buffer
//     strncpy(ret + extra_len, str, j);
//     ret[extra_len + j] = '\0';

//     return ret;
// }


// Token *tokenize_input(char *input)
// {
//     Token *head = NULL;
//     Token *tail = NULL;
//     int i = 0;
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
//             if (input[i] == '\'' || (input[i] != ' ' && input[i] != '\t' && input[i] != '\0'))
//             {
//                 xtra = new_token(TOKEN_MERGE_FLAG,"YES");
//                 //printf("here\n");
//             }
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
//             while (!isspace(input[i]) && input[i] != '|' && input[i] != '<' && input[i] != '>' && input[i] != '\'' && input[i] != '\0') {i++;}
//             // if (input[i] == '\'')
//             //     xtra = new_token(TOKEN_MERGE_FLAG,"YES");
//             char *word = strndup(&input[start], i - start);
//             new_t = new_token(TOKEN_WORD, word);
//             if (input[i] == '\'')
//             {
//                 xtra = new_token(TOKEN_MERGE_FLAG,"YES");
//             }
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

// void free_tokens(Token *tokens) 
// {
//     Token *current = tokens;
//     while (current)
//     {
//         Token *next = current->next;
//         free(current->value);
//         free(current);
//         current = next;
//     }
// }


// // Token *tokenize_input(char *input)
// // {
// //     Token *head = NULL;
// //     Token *tail = NULL;
// //     char *segment = NULL;
// //     int i = 0;

// //     while (input[i] != '\0')
// //     {
// //         if (isspace(input[i]))
// //         {
// //             i++;
// //             continue;
// //         }

// //         Token *new_t = NULL;

// //         // Handle pipes
// //         if (input[i] == '|')
// //         {
// //             new_t = new_token(TOKEN_PIPE, "|");
// //             i++;
// //         }
// //         // Handle input and output redirection
// //         else if (input[i] == '<') 
// //         {
// //             if (input[i + 1] == '<') 
// //             {
// //                 new_t = new_token(TOKEN_HEREDOC, "<<");
// //                 i += 2;
// //             } 
// //             else 
// //             {
// //                 new_t = new_token(TOKEN_INPUT_REDIRECT, "<");
// //                 i++;
// //             }
// //         }
// //         else if (input[i] == '>')
// //         {
// //             if (input[i + 1] == '>')
// //             {
// //                 new_t = new_token(TOKEN_APPEND_REDIRECT, ">>");
// //                 i += 2;
// //             }
// //             else
// //             {
// //                 new_t = new_token(TOKEN_OUTPUT_REDIRECT, ">");
// //                 i++;
// //             }
// //         }
// //         // Handle double quotes
// //         else if (input[i] == '"') 
// //         {
// //             i++;  // Skip opening quote
// //             segment = ft_word(&input[i], segment);
// //             if (!segment)
// //                 return new_token(TOKEN_ERROR, "Memory allocation failed");

// //             // Move `i` past the quoted content
// //             while (input[i] != '"' && input[i] != '\0') i++;
// //             if (input[i] == '"')
// //                 i++;  // Skip closing quote

// //             new_t = new_token(TOKEN_WORD, segment);
// //             free(segment);
// //             segment = NULL;
// //         }
// //         // Handle single quotes
// //         else if (input[i] == '\'') 
// //         {
// //             i++;  // Skip opening quote
// //             int start = i;

// //             while (input[i] != '\'' && input[i] != '\0') i++;
// //             if (input[i] == '\0')
// //                 return new_token(TOKEN_ERROR, "Unclosed single quote");

// //             char *quote_str = strndup(&input[start], i - start);
// //             if (!quote_str)
// //                 return new_token(TOKEN_ERROR, "Memory allocation failed");

// //             new_t = new_token(TOKEN_QUOTE, quote_str);
// //             free(quote_str);

// //             i++;  // Skip closing quote
// //         }
// //         // Handle environment variables and exit status
// //         else if (input[i] == '$')
// //         {
// //             i++;  // Skip '$'
// //             if (input[i] == '?') 
// //             {
// //                 i++;
// //                 new_t = new_token(TOKEN_EXIT_STATUS, "0");
// //             }
// //             else
// //             {
// //                 int start = i;
// //                 while (isalnum(input[i]) || input[i] == '_') i++;

// //                 char *env_var = strndup(&input[start], i - start);
// //                 if (!env_var)
// //                     return new_token(TOKEN_ERROR, "Memory allocation failed");

// //                 new_t = new_token(TOKEN_ENV_VAR, env_var);
// //                 free(env_var);
// //             }
// //         }
// //         // Handle regular words
// //         else
// //         {
// //             int start = i;
// //             while (!isspace(input[i]) && input[i] != '|' && input[i] != '<' &&
// //                    input[i] != '>' && input[i] != '\'' && input[i] != '"' && input[i] != '$' &&
// //                    input[i] != '\0')
// //                 i++;

// //             char *word = strndup(&input[start], i - start);
// //             if (!word)
// //                 return new_token(TOKEN_ERROR, "Memory allocation failed");

// //             new_t = new_token(TOKEN_WORD, word);
// //             free(word);
// //         }

// //         // Append the new token to the list
// //         if (new_t)
// //         {
// //             if (tail)
// //                 tail->next = new_t;
// //             else
// //                 head = new_t;
// //             tail = new_t;
// //         }
// //     }

// //     return head;
// // }



// // // char *ft_word(char *str, char *extra)  //ie segment new token (token word segment)
// // // {
// // //     static int i = 0;
// // //     int j = 0;
// // //     char *ret = NULL;
// // //     // char *str_dup = strdup(str);
// // //     while(str[j] != '"' && str[j] != '\0')
// // //         j++;
// // //     // i = i + j;
// // //     ret = (char *)realloc(extra, (i + j) * sizeof(char));
// // //     if (ret == NULL)
// // //     {
// // //         perror("realloc failed");
// // //         return NULL;
// // //     }
// // //     j = 0;
// // //     while(str[j] != '"' && str[j] != '\0')
// // //     {
// // //         ret[i] = str[j];
// // //         i++;
// // //         j++;
// // //     }
// // //     ret[i] = '\0';
// // //     // free(str);
// // //     return ret;
// // // }

// // // Token *tokenize_input(char *input)
// // // {
// // //     Token *head = NULL;
// // //     Token *tail = NULL;
// // //     int cpy = 0;
// // //     int i = 0;
// // //     int flag = 0;
// // //     Token *xtra = NULL;
// // //     char *segment = NULL;

// // //     while (input[i] != '\0')
// // //     {
// // //         if (isspace(input[i]))
// // //         {
// // //             i++;
// // //             continue;
// // //         }

// // //         Token *new_t = NULL;

// // //         if (input[i] == '|')
// // //         {
// // //             new_t = new_token(TOKEN_PIPE, "|");
// // //             i++;
// // //         }
// // //         else if (input[i] == '<') 
// // //         {
// // //             if (input[i + 1] == '<') 
// // //             {
// // //                 new_t = new_token(TOKEN_HEREDOC, "<<");
// // //                 i += 2;
// // //             } 
// // //             else 
// // //             {
// // //                 new_t = new_token(TOKEN_INPUT_REDIRECT, "<");
// // //                 i++;
// // //             }
// // //         }
// // //         else if (input[i] == '>')
// // //         {
// // //             if (input[i + 1] == '>')
// // //             {
// // //                 new_t = new_token(TOKEN_APPEND_REDIRECT, ">>");
// // //                 i += 2;
// // //             }
// // //             else
// // //             {
// // //                 new_t = new_token(TOKEN_OUTPUT_REDIRECT, ">");
// // //                 i++;
// // //             }
// // //         }
// // //         else if(tail != NULL && input[i] != '"' && (tail->type == TOKEN_DOUBLE_QUOTE || tail->next->type == TOKEN_DOUBLE_QUOTE))
// // //         {
// // //             if (segment == NULL)
// // //                 segment = ft_strdup(&input[i]);
// // //             segment = ft_word(&input[i],segment);
// // //             i = cpy;
// // //             if (input[i] == '\0')
// // //                 new_t = new_token(TOKEN_WORD, segment);
// // //             // i = cpy;
// // //         }
// // //         // Handle quoted strings
// // //         else if (input[i] == '"') 
// // //         {
// // //             char *proc_inp = handle_double(&input[i]);
// // //             if (!proc_inp)
// // //                 return new_token(TOKEN_ERROR, "Unclosed double quotes");
// // //             input = proc_inp;
// // //             cpy = i;
// // //             i = 0;
// // //             new_t = new_token(TOKEN_DOUBLE_QUOTE, "");
// // //         }
// // //         //Single quotes
// // //         else if (input[i] == '\'')
// // //         {
// // //             char quote_char = input[i++];
// // //             int start = i;
// // //             while (input[i] != quote_char && input[i] != '\0') i++;
// // //             if (input[i] == '\0')
// // //             {
// // //                 perror("minishell: unclosed quote");
// // //                 return new_token(TOKEN_ERROR, "Unclosed quote");
// // //             }
// // //             char *quote_str = strndup(&input[start], i - start);
// // //             new_t = new_token(TOKEN_QUOTE, quote_str);
// // //             free(quote_str);
// // //             i++;
// // //             if (input[i] == '\'' || (input[i] != ' ' && input[i] != '\t' && input[i] != '\0'))
// // //             {
// // //                 xtra = new_token(TOKEN_MERGE_FLAG,"YES");
// // //                 //printf("here\n");
// // //             }
// // //         }

// // //         // Handle environment variables and exit status 
// // //         else if (input[i] == '$')
// // //         {
// // //             if (input[++i] == '?')
// // //             {
// // //                 i++;
// // //                 char *default_status = strdup("0");
// // //                 new_t = new_token(TOKEN_EXIT_STATUS, default_status);
// // //                 free(default_status);
// // //             }
// // //             else
// // //             {
// // //                 int start = i;
// // //                 while (isalnum(input[i]) || input[i] == '_') i++;
// // //                 char *env_var = strndup(&input[start], i - start);
// // //                 new_t = new_token(TOKEN_ENV_VAR, env_var);
                
// // //                 free(env_var);
// // //             }
// // //         }
// // //         // Handle normal words
// // //         else
// // //         {
            
// // //             int start = i;
// // //             while (!isspace(input[i]) && input[i] != '|' && input[i] != '<' && input[i] != '>' && input[i] != '\'' && input[i] != '\0') {i++;}
// // //             // if (input[i] == '\'')
// // //             //     xtra = new_token(TOKEN_MERGE_FLAG,"YES");
// // //             char *word = strndup(&input[start], i - start);
// // //             new_t = new_token(TOKEN_WORD, word);
// // //             if (input[i] == '\'')
// // //             {
// // //                 xtra = new_token(TOKEN_MERGE_FLAG,"YES");
// // //             }
// // //             free(word);
// // //         }

// // //         // Append the new token to the token list
// // //         if (new_t && flag != 1) 
// // //         {
// // //             if (tail) tail->next = new_t;
// // //             else head = new_t;
// // //             tail = new_t;
// // //         }
// // //         else if (flag == 1)
// // //             flag = 0;
// // //         if (xtra != NULL)
// // //         {
// // //             if (tail) tail->next = xtra;
// // //             else head = xtra;
// // //             tail = xtra;
// // //             xtra = NULL;
// // //         }
// // //     }
// // //     // if (input[i] == '\0' && head == NULL)
// // //     //     head->type = TOKEN_EMPTY;
// // //     return (head);
// // // }
// // // Token *tokenize_input(char *input) {
// // //     Token *head = NULL, *tail = NULL;
// // //     int i = 0;

// // //     while (input[i] != '\0') {
// // //         // Skip whitespace
// // //         if (isspace(input[i])) {
// // //             i++;
// // //             continue;
// // //         }

// // //         Token *new_t = NULL;

// // //         // Handle double quotes
// // //         if (input[i] == '"') {
// // //             i++; // Skip the opening quote
// // //             char *buffer = calloc(1, 1); // Empty buffer for quoted content
// // //             while (input[i] != '\0') {
// // //                 if (input[i] == '"') {
// // //                     // Handle consecutive quotes
// // //                     if (input[i + 1] == '"') {
// // //                         char *tmp = buffer;
// // //                         buffer = realloc(buffer, strlen(buffer) + 1 + 1); // +1 for empty string
// // //                         strcat(buffer, ""); // Append empty string
// // //                         free(tmp);
// // //                         i += 2; // Skip both quotes
// // //                     } else {
// // //                         i++; // Skip closing quote
// // //                         break;
// // //                     }
// // //                 } else {
// // //                     // Regular character inside quotes
// // //                     size_t len = strlen(buffer);
// // //                     char *tmp = buffer;
// // //                     buffer = realloc(buffer, len + 2); // +1 for char, +1 for null terminator
// // //                     buffer[len] = input[i];
// // //                     buffer[len + 1] = '\0';
// // //                     free(tmp);
// // //                     i++;
// // //                 }
// // //             }

// // //             // Error for unclosed quotes
// // //             if (input[i] == '\0') {
// // //                 free(buffer);
// // //                 fprintf(stderr, "minishell: unclosed double quote\n");
// // //                 free_tokens(head);
// // //                 return NULL;
// // //             }

// // //             new_t = new_token(TOKEN_WORD, buffer);
// // //             free(buffer);
// // //         }
// // //         // Handle regular words
// // //         else {
// // //             int start = i;
// // //             while (!isspace(input[i]) && input[i] != '"' && input[i] != '\0')
// // //                 i++;
// // //             char *word = strndup(&input[start], i - start);
// // //             new_t = new_token(TOKEN_WORD, word);
// // //             free(word);
// // //         }

// // //         // Append new token to the list
// // //         if (new_t) {
// // //             if (tail)
// // //                 tail->next = new_t;
// // //             else
// // //                 head = new_t;
// // //             tail = new_t;
// // //         }
// // //     }

// // //     return head;
// // // }

// // // Print the tokens for debugging
// // // void print_tokens(Token *head) {
// // //     while (head) {
// // //         printf("Token: [%s]\n", head->value);
// // //         head = head->next;
// // //     }
// // // }


// // void free_tokens(Token *tokens) 
// // {
// //     Token *current = tokens;
// //     while (current)
// //     {
// //         Token *next = current->next;
// //         free(current->value);
// //         free(current);
// //         current = next;
// //     }
// // }
