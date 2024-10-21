#ifndef TOKENS_H
#define TOKENS_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "minishell.h"

#define TOKEN_WORD 1
#define TOKEN_PIPE 2
#define TOKEN_INPUT_REDIRECT 3
#define TOKEN_OUTPUT_REDIRECT 4
#define TOKEN_APPEND_REDIRECT 5
#define TOKEN_HEREDOC 6
#define TOKEN_QUOTE 7
#define TOKEN_DOUBLE_QUOTE 8
#define TOKEN_ENV_VAR 9
#define TOKEN_EXIT_STATUS 10
#define TOKEN_EOF 11
#define TOKEN_ERROR 12
#define TOKEN_MERGE_FLAG 13


typedef struct Token
{
    int type;
    char *value;
    struct Token *next;
} Token;

Token *new_token(int type, char *value);
Token *tokenize_input(char *input);
void free_tokens(Token *tokens);

#endif
