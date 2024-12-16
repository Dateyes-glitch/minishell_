#include "minishell.h"

// Function to count the number of double quotes in the input string
int count_quotes(const char *input) {
    int quote_count = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] == '"') {
            quote_count++;
        }
    }
    return quote_count;
}
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

Token *tokenize_input(char *input) {
    Token *head = NULL;
    Token *tail = NULL;
    int i = 0;
    int flag = 0;
    Token *xtra = NULL;

    // Check if the number of double quotes is even or odd
    if (count_quotes(input) % 2 != 0) {
        perror("minishell: unmatched double quote");
        return new_token(TOKEN_ERROR, "Unmatched quote");
    }

    while (input[i] != '\0') {
        if (isspace(input[i])) {
            i++;
            continue;
        }

        Token *new_t = NULL;

        // Handle pipes
        if (input[i] == '|') {
            new_t = new_token(TOKEN_PIPE, "|");
            i++;
        }
        // Handle input redirection
        else if (input[i] == '<') {
            if (input[i + 1] == '<') {
                new_t = new_token(TOKEN_HEREDOC, "<<");
                i += 2;
            } else {
                new_t = new_token(TOKEN_INPUT_REDIRECT, "<");
                i++;
            }
        }
        // Handle output redirection
        else if (input[i] == '>') {
            if (input[i + 1] == '>') {
                new_t = new_token(TOKEN_APPEND_REDIRECT, ">>");
                i += 2;
            } else {
                new_t = new_token(TOKEN_OUTPUT_REDIRECT, ">");
                i++;
            }
        }
        // Handle double quotes (quoted strings)
        else if (input[i] == '"') {
            i++; // Skip the opening quote
            int start = i; // Start index of quoted content

            // Loop through characters inside the quotes
            while (input[i] != '\0') {
                if (input[i] == '"') {
                    // Found closing quote, extract the string inside the quotes
                    break;
                }
                i++; // Move to the next character
            }

            // If we encounter a closing quote, extract the string between the quotes
            if (input[i] == '"') {
                char *quoted_content = strndup(&input[start], i - start);
                new_t = new_token(TOKEN_WORD, quoted_content);
                free(quoted_content);
                i++; // Skip the closing quote
            } else {
                // If no closing quote is found, return an error
                perror("minishell: unmatched double quote");
                return new_token(TOKEN_ERROR, "Unmatched quote");
            }
        }
        // Handle single quotes (for completeness)
        else if (input[i] == '\'') {
            i++; // Skip the opening single quote
            int start = i;
            while (input[i] != '\'' && input[i] != '\0') i++;
            if (input[i] == '\0') {
                perror("minishell: unclosed single quote");
                return new_token(TOKEN_ERROR, "Unclosed single quote");
            }
            char *quote_str = strndup(&input[start], i - start);
            new_t = new_token(TOKEN_QUOTE, quote_str);
            free(quote_str);
            i++;
            if (input[i] == '\'' || (input[i] != ' ' && input[i] != '\t' && input[i] != '\0')) {
                xtra = new_token(TOKEN_MERGE_FLAG, "YES");
            }
        }
        // Handle environment variables and exit status
        else if (input[i] == '$') {
            if (input[++i] == '?') {
                i++;
                char *default_status = strdup("0");
                new_t = new_token(TOKEN_EXIT_STATUS, default_status);
                free(default_status);
            } else {
                int start = i;
                while (isalnum(input[i]) || input[i] == '_') i++;
                char *env_var = strndup(&input[start], i - start);
                new_t = new_token(TOKEN_ENV_VAR, env_var);
                free(env_var);
            }
        }
        // Handle regular words
        else {
            int start = i;
            while (!isspace(input[i]) && input[i] != '|' && input[i] != '<' && input[i] != '>' && input[i] != '\'' && input[i] != '\0') {
                i++;
            }
            char *word = strndup(&input[start], i - start);
            new_t = new_token(TOKEN_WORD, word);
            free(word);
        }

        // Append the new token to the token list
        if (new_t && flag != 1) {
            if (tail) tail->next = new_t;
            else head = new_t;
            tail = new_t;
        } else if (flag == 1) {
            flag = 0;
        }

        // Handle extra merge flag (if applicable)
        if (xtra != NULL) {
            if (tail) tail->next = xtra;
            else head = xtra;
            tail = xtra;
            xtra = NULL;
        }
    }

    return head;
}

void free_tokens(Token *tokens) {
    Token *current = tokens;
    while (current) {
        Token *next = current->next;
        free(current->value);
        free(current);
        current = next;
    }
}
