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
//     char *segment = NULL;
//     int i = 0;

//     while (input[i] != '\0')
//     {
//         if (isspace(input[i]))
//         {
//             i++;
//             continue;
//         }

//         Token *new_t = NULL;

//         // Handle pipes
//         if (input[i] == '|')
//         {
//             new_t = new_token(TOKEN_PIPE, "|");
//             i++;
//         }
//         // Handle input and output redirection
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
//          // Handle double quotes
//         else if (input[i] == '"') {
//             i++; // Skip the opening quote
//             char *buffer = calloc(1, 1); // Empty buffer for quoted content
//             while (input[i] != '\0') {
//                 if (input[i] == '"') {
//                     // Handle consecutive quotes
//                     if (input[i + 1] == '"') {
//                         char *tmp = buffer;
//                         buffer = realloc(buffer, strlen(buffer) + 1 + 1); // +1 for empty string
//                         strcat(buffer, ""); // Append empty string
//                         free(tmp);
//                         i += 2; // Skip both quotes
//                     } else {
//                         i++; // Skip closing quote
//                         break;
//                     }
//                 } else {
//                     // Regular character inside quotes
//                     size_t len = strlen(buffer);
//                     char *tmp = buffer;
//                     buffer = realloc(buffer, len + 2); // +1 for char, +1 for null terminator
//                     buffer[len] = input[i];
//                     buffer[len + 1] = '\0';
//                     free(tmp);
//                     i++;
//                 }
//             }

//             // Error for unclosed quotes
//             if (input[i] == '\0') {
//                 free(buffer);
//                 fprintf(stderr, "minishell: unclosed double quote\n");
//                 free_tokens(head);
//                 return NULL;
//             }

//             new_t = new_token(TOKEN_WORD,buffer);
//             free(buffer);
//         }
//         // Handle single quotes
//         else if (input[i] == '\'') 
//         {
//             i++;  // Skip opening quote
//             int start = i;

//             while (input[i] != '\'' && input[i] != '\0') i++;
//             if (input[i] == '\0')
//                 return new_token(TOKEN_ERROR, "Unclosed single quote");

//             char *quote_str = strndup(&input[start], i - start);
//             if (!quote_str)
//                 return new_token(TOKEN_ERROR, "Memory allocation failed");

//             new_t = new_token(TOKEN_QUOTE, quote_str);
//             free(quote_str);

//             i++;  // Skip closing quote
//         }
//         // Handle environment variables and exit status
//         else if (input[i] == '$')
//         {
//             i++;  // Skip '$'
//             if (input[i] == '?') 
//             {
//                 i++;
//                 new_t = new_token(TOKEN_EXIT_STATUS, "0");
//             }
//             else
//             {
//                 int start = i;
//                 while (isalnum(input[i]) || input[i] == '_') i++;

//                 char *env_var = strndup(&input[start], i - start);
//                 if (!env_var)
//                     return new_token(TOKEN_ERROR, "Memory allocation failed");

//                 new_t = new_token(TOKEN_ENV_VAR, env_var);
//                 free(env_var);
//             }
//         }
//         // Handle regular words
//         else
//         {
//             int start = i;
//             while (!isspace(input[i]) && input[i] != '|' && input[i] != '<' &&
//                    input[i] != '>' && input[i] != '\'' && input[i] != '"' && input[i] != '$' &&
//                    input[i] != '\0')
//                 i++;

//             char *word = strndup(&input[start], i - start);
//             if (!word)
//                 return new_token(TOKEN_ERROR, "Memory allocation failed");

//             new_t = new_token(TOKEN_WORD, word);
//             free(word);
//         }

//         // Append the new token to the list
//         if (new_t)
//         {
//             if (tail)
//                 tail->next = new_t;
//             else
//                 head = new_t;
//             tail = new_t;
//         }
//     }

//     return head;
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
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <ctype.h>

// // Define token types for better clarity
// #define TOKEN_WORD 0
// #define TOKEN_PIPE 1
// #define TOKEN_REDIRECT_IN 2
// #define TOKEN_REDIRECT_OUT 3
// #define TOKEN_APPEND_OUT 4
// #define TOKEN_ERROR -1

// typedef struct Token {
//     char *value;
//     int type; // Token type (word, pipe, etc.)
//     struct Token *next;
// } Token;

// // Utility: Free token list
// void free_tokens(Token *head) {
//     while (head) {
//         Token *temp = head;
//         free(head->value);
//         head = head->next;
//         free(temp);
//     }
// }

// // Utility: Add a token to the list
// void add_token(Token **head, Token **tail, char *value, int type) {
//     Token *new_token = malloc(sizeof(Token));
//     if (!new_token) {
//         perror("malloc");
//         return;
//     }
//     new_token->value = strdup(value);
//     if (!new_token->value) {
//         perror("strdup");
//         free(new_token);
//         return;
//     }
//     new_token->type = type;
//     new_token->next = NULL;

//     if (!*head)
//         *head = new_token;
//     else
//         (*tail)->next = new_token;

//     *tail = new_token;
// }

// // Utility: Check for unclosed quotes
// int check_unclosed_quotes(const char *input) {
//     int double_quotes = 0;
//     int single_quotes = 0;
    
//     for (int i = 0; input[i]; i++) {
//         if (input[i] == '"') {
//             double_quotes++;
//         } else if (input[i] == '\'') {
//             single_quotes++;
//         }
//     }

//     // Check if there is an odd number of quotes for either
//     if (double_quotes % 2 != 0 || single_quotes % 2 != 0) {
//         return 1; // There are unclosed quotes
//     }
//     return 0; // Quotes are balanced
// }

// // Tokenizer function
// Token *tokenize_input(const char *input) {
//     if (!input)
//         return NULL;

//     // Check for unclosed quotes
//     if (check_unclosed_quotes(input)) {
//         fprintf(stderr, "minishell: unclosed quote\n");
//         return NULL;
//     }

//     Token *head = NULL, *tail = NULL;
//     int i = 0;

//     while (input[i]) {
//         // Skip whitespace
//         if (isspace(input[i])) {
//             i++;
//             continue;
//         }

//         // Handle pipes
//         if (input[i] == '|') {
//             add_token(&head, &tail, "|", TOKEN_PIPE);
//             i++;
//             continue;
//         }

//         // Handle redirections: >, >>, <
//         if (input[i] == '>') {
//             if (input[i + 1] == '>') {
//                 add_token(&head, &tail, ">>", TOKEN_APPEND_OUT);
//                 i += 2;
//             } else {
//                 add_token(&head, &tail, ">", TOKEN_REDIRECT_OUT);
//                 i++;
//             }
//             continue;
//         }
//         if (input[i] == '<') {
//             add_token(&head, &tail, "<", TOKEN_REDIRECT_IN);
//             i++;
//             continue;
//         }

//         // Handle quoted strings (handling consecutive quotes correctly)
//         if (input[i] == '"' || input[i] == '\'') {
//             char quote_type = input[i];
//             int start = ++i; // Skip opening quote
//             while (input[i] && input[i] != quote_type) {
//                 if (input[i] == quote_type && input[i+1] == quote_type) { 
//                     // Skip consecutive quotes within a quote
//                     i++;
//                 }
//                 i++;
//             }
//             if (input[i] == '\0') {
//                 fprintf(stderr, "minishell: unclosed %c quote\n", quote_type);
//                 free_tokens(head);
//                 return NULL;
//             }
//             char *quoted_str = strndup(&input[start], i - start);
//             if (!quoted_str) {
//                 perror("strndup");
//                 free_tokens(head);
//                 return NULL;
//             }
//             add_token(&head, &tail, quoted_str, TOKEN_WORD);
//             free(quoted_str); // Free temporary string
//             i++; // Skip closing quote
//             continue;
//         }

//         // Handle regular words
//         int start = i;
//         while (input[i] && !isspace(input[i]) && input[i] != '|' &&
//                input[i] != '<' && input[i] != '>' && input[i] != '"' && input[i] != '\'')
//             i++;
//         char *word = strndup(&input[start], i - start);
//         if (!word) {
//             perror("strndup");
//             free_tokens(head);
//             return NULL;
//         }
//         add_token(&head, &tail, word, TOKEN_WORD);
//         free(word); // Free temporary string
//     }

//     return head;
// }

// // // Utility: Print tokens for debugging
// // void print_tokens(Token *head) {
// //     while (head) {
// //         const char *type_str = "";
// //         switch (head->type) {
// //             case TOKEN_WORD: type_str = "WORD"; break;
// //             case TOKEN_PIPE: type_str = "PIPE"; break;
// //             case TOKEN_REDIRECT_IN: type_str = "REDIRECT_IN"; break;
// //             case TOKEN_REDIRECT_OUT: type_str = "REDIRECT_OUT"; break;
// //             case TOKEN_APPEND_OUT: type_str = "APPEND_OUT"; break;
// //             case TOKEN_ERROR: type_str = "ERROR"; break;
// //         }
// //         printf("Token: '%s' (Type: %s)\n", head->value, type_str);
// //         head = head->next;
// //     }
// // }

// // // Main function for testing
// // int main() {
// //     const char *input = "echo \"\"\"\"sandy\"\"\"\""; // Test input with consecutive quotes
// //     Token *tokens = tokenize_input(input);
// //     if (tokens) {
// //         print_tokens(tokens);
// //         free_tokens(tokens);
// //     }
// //     return 0;
// // }
 // Handle double quotes

 ////USE THIS
        // if (input[i] == '"') {
        //     i++; // Skip the opening quote
        //     char *buffer = calloc(1, 1); // Empty buffer for quoted content
        //     while (input[i] != '\0') {
        //         if (input[i] == '"') {
        //             // Handle consecutive quotes
        //             if (input[i + 1] == '"') {
        //                 char *tmp = buffer;
        //                 buffer = realloc(buffer, strlen(buffer) + 1 + 1); // +1 for empty string
        //                 strcat(buffer, ""); // Append empty string
        //                 free(tmp);
        //                 i += 2; // Skip both quotes
        //             } else {
        //                 i++; // Skip closing quote
        //                 break;
        //             }
        //         } else {
        //             // Regular character inside quotes
        //             size_t len = strlen(buffer);
        //             char *tmp = buffer;
        //             buffer = realloc(buffer, len + 2); // +1 for char, +1 for null terminator
        //             buffer[len] = input[i];
        //             buffer[len + 1] = '\0';
        //             free(tmp);
        //             i++;
        //         }
        //     }