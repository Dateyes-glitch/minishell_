#include "minishell.h"


Command *new_command() 
{
    Command *cmd = malloc(sizeof(Command));
    if (!cmd) 
    {
        perror("minishell: malloc");
        exit(EXIT_FAILURE);
    }
    cmd->args = NULL;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append = 0;
    cmd->next = NULL;
    cmd->heredoc = 0;
    cmd->heredoc_delim = NULL;
    return cmd;
}

void add_argument(Command *cmd, char *arg) 
{
    int count = 0;
    while (cmd->args && cmd->args[count]) count++;
    cmd->args = realloc(cmd->args, (count + 2) * sizeof(char *));
    if (!cmd->args)
    {
        perror("minishell: realloc");
        exit(EXIT_FAILURE);
    }
    cmd->args[count] = strdup(arg);
    cmd->args[count + 1] = NULL;
}

Command *parse_pipeline(Token **tokens, envvar **env_list)
{
    Command *head = NULL;
    Command *tail = NULL;
    Command *current_cmd = NULL;

    Token *current_token = *tokens;
    while (current_token) 
    {
        if (current_token->type == TOKEN_HEREDOC && head == NULL)
        {
            if (!current_cmd) 
            {
                current_cmd = new_command();
                if (!head) 
                    head = current_cmd;
                else 
                    tail->next = current_cmd;
                tail = current_cmd;
            }
            current_cmd->heredoc = 126;
            current_token->value = strdup("heredoc_file.txt");
            current_token->type = TOKEN_WORD;
        }
        if (current_token->type == TOKEN_WORD || current_token->type == TOKEN_QUOTE || current_token->type == TOKEN_DOUBLE_QUOTE || current_token->type == TOKEN_ENV_VAR) 
        {
            // If there's no current command, start a new one
            if (!current_cmd) 
            {
                current_cmd = new_command();
                if (!head) 
                    head = current_cmd;
                else 
                    tail->next = current_cmd;
                tail = current_cmd;
            }
            if (current_token->type == TOKEN_ENV_VAR)
            {
                char *env_value = get_env_var(env_list, current_token->value);
                if (env_value)
                    current_token->value = strdup(env_value);
                else
                    current_token->value = strdup("");
            }
            add_argument(current_cmd, current_token->value);
        } 
        else if (current_token->type == TOKEN_PIPE) 
        {
            if (current_cmd) 
            {
                tail = current_cmd;
                current_cmd = NULL;
            }
        } 
        else if (current_token->type == TOKEN_INPUT_REDIRECT) {
            if (!current_cmd) 
            {
                // Ensure a current command exists
                perror("minishell: syntax error near unexpected token '<'");
                return NULL;
            }
            current_token = current_token->next;
            if (!current_token || (current_token->type != TOKEN_WORD && current_token->type != TOKEN_QUOTE && current_token->type != TOKEN_DOUBLE_QUOTE))
            {
                perror("minishell: syntax error near unexpected token '<'");
                return NULL;
            }
            current_cmd->input_file = strdup(current_token->value);
        } 
        else if (current_token->type == TOKEN_OUTPUT_REDIRECT || current_token->type == TOKEN_APPEND_REDIRECT) 
        {
            if (!current_cmd) 
            {
                // Ensure a current command exists
                perror("minishell: syntax error near unexpected token '>' or '>>'");
                return NULL;
            }
            current_cmd->append = (current_token->type == TOKEN_APPEND_REDIRECT);
            current_token = current_token->next;
            if (!current_token || (current_token->type != TOKEN_WORD && current_token->type != TOKEN_QUOTE && current_token->type != TOKEN_DOUBLE_QUOTE))
            {
                perror("minishell: syntax error near unexpected token '>' or '>>'");
                return NULL;
            }
            current_cmd->output_file = strdup(current_token->value);
        }
        current_token = current_token->next;
    }

    *tokens = current_token;
    return head;
}

void free_commands(Command *commands) 
{
    Command *current = commands;

    while (current) 
    {
        Command *next = current->next;
        if (current->args) 
        {
            for (int i = 0; current->args[i]; i++)
            {
                free(current->args[i]);
            }
            free(current->args);
        }
        if (current->input_file)
            free(current->input_file);
        if (current->output_file)
            free(current->output_file);
        //free(current);
        current = next;
    }
}
