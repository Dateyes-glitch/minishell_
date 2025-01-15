/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 20:56:35 by hawild            #+#    #+#             */
/*   Updated: 2025/01/15 17:03:59 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void handle_token_env_var(envvar **env_list, Token *current_token, Command *current_cmd)
{
    char *env_value;

    env_value = get_env_var(env_list, current_token->value);
    if (env_value)
        current_token->value = ft_strdup(env_value);
    else
        current_token->value = ft_strdup("dontadd");
    if (!current_cmd->args && env_value)
        current_cmd->env_exp = strdup(env_value);
    if (!env_value)
        current_cmd->env_exp = strdup(":still:");
}

// void handle_token_merge_flag(Token **current_token, shell_status **e_status)
// {
//     char *merged_arg;
//     char *temp;

//     merged_arg = NULL;
//     if (strcmp((*current_token)->value, "dontadd") != 0)
//         merged_arg = ft_strdup((*current_token)->value);
//     else
//     {
//         merged_arg = ft_strdup("");
//         *current_token = (*current_token)->next->next;
//         if ((*current_token)->type == TOKEN_EXIT_STATUS)
//             (*current_token)->value = ft_itoa((*e_status)->last_exit_status);
//         else if (strcmp((*current_token)->value, "dontadd") == 0)
//             (*current_token)->value = ft_strdup("");
//         temp = merged_arg;
//         merged_arg = ft_strjoin(temp, (*current_token)->value);
//         free(temp);

//         (*current_token)->value = ft_strdup(merged_arg);
//         (*current_token)->type = TOKEN_WORD; 
//         free(merged_arg);
//     }
// }

// void handle_token_merge_flag(Token *current_token, shell_status *e_status)
// {
//     char *merged_arg = NULL;
//     char *temp = NULL;

//     if (strcmp(current_token->value, "dontadd") != 0)
//     {
//         merged_arg = ft_strdup(current_token->value);
//     }
//     else
//     {
//         merged_arg = ft_strdup("");
//         current_token = current_token->next->next; // This is safe inside the function.

//         if (current_token->type == TOKEN_EXIT_STATUS)
//             current_token->value = ft_itoa(e_status->last_exit_status);
//         else if (strcmp(current_token->value, "dontadd") == 0)
//             current_token->value = ft_strdup("");
        
//         temp = merged_arg;
//         merged_arg = ft_strjoin(temp, current_token->value);
//         free(temp);
//     }

//     current_token->value = ft_strdup(merged_arg);
//     current_token->type = TOKEN_WORD;
//     free(merged_arg);
// }


int handle_token_input_redirect(Command *current_cmd, Token **current_token)
{
    if (!current_cmd) 
    {
        perror("minishell: syntax error near unexpected token '<'");
        return (1);
    }
    if (current_cmd->output_file != NULL)
        current_cmd->output = 1;
    *current_token = (*current_token)->next; // Update the pointer in the caller
    if (!(*current_token) || ((*current_token)->type != TOKEN_WORD && 
                               (*current_token)->type != TOKEN_QUOTE &&
                               (*current_token)->type != TOKEN_DOUBLE_QUOTE))
    {
        perror("minishell|: syntax error near unexpected token '<'");
        return (1);
    }
    current_cmd->input_file = ft_strdup((*current_token)->value);
    calculate(current_cmd);
    return (0);
}

int handle_token_output_append(Command *current_cmd, Token **current_token)
{
    int fd;

    if (!current_cmd) 
    {
        perror("minishell: syntax error near unexpected token '>' or '>>'");
        return (1);
    }
    if (current_cmd->output_file)
        fd = open(current_cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    current_cmd->append = ((*current_token)->type == TOKEN_APPEND_REDIRECT);
    *current_token = (*current_token)->next;
    if (!(*current_token) || ((*current_token)->type != TOKEN_WORD &&
                              (*current_token)->type != TOKEN_QUOTE &&
                              (*current_token)->type != TOKEN_DOUBLE_QUOTE))
    {
        perror("minishell: syntax error near unexpected token '>' or '>>'");
        return (1);
    }
    current_cmd->output_file = strdup((*current_token)->value);
    return (0);
}

// int     handle_token_pipe(Token *current_token, Command *current_cmd, Command **tail)
// {
//     if (!current_token->next)
//     {
//         perror("minishell: syntax cannot end with pipe");
//         return (1);
//     }
//     if (current_cmd) 
//     {
//         (*tail) = current_cmd;
//         current_cmd = NULL;
//         return (0);
//     }
//     return (0);
// }

void handle_token_heredoc(Token **current_token, Command **current_cmd, Command **head, Command **tail)
{
    if (!(*current_cmd))
    {
        *current_cmd = new_command();
        if (!(*head))
            *head = *current_cmd;
        if (*tail)
            (*tail)->next = *current_cmd;
        *tail = *current_cmd;
    }
    *current_token = (*current_token)->next;
    if (!(*current_token) || (*current_token)->type != TOKEN_WORD)
    {
        fprintf(stderr, "minishell: syntax error near unexpected token '<<'\n");
        return;
    }
    (*current_cmd)->heredoc_delim = strdup((*current_token)->value);
}

int handle_token_pipe(Token **current_token, Command **current_cmd, Command **tail)
{
    // Ensure there is a next token; otherwise, it's a syntax error
    if (!(*current_token)->next) {
        fprintf(stderr, "minishell: syntax error near unexpected token '|'\n");
        return 1;
    }

    // If there is an active command, finalize it and prepare for a new one
    if (*current_cmd)
    {
        *tail = *current_cmd;
        *current_cmd = NULL;
    }

    return 0;
}

void initialize_new_command(Command **current_cmd, Command **head, Command **tail)
{
    *current_cmd = new_command();
    if (!*head) 
        *head = *current_cmd;
    else 
        (*tail)->next = *current_cmd;
    *tail = *current_cmd;
}


Command *parse_pipeline(Token **tokens, envvar **env_list, shell_status *e_status)
{
    Command *head;
    Command *tail;
    Command *current_cmd;
    Token *current_token;
    int flag;

    current_token = *tokens;
    head = NULL;
    tail = NULL;
    current_cmd = NULL;
    flag = 0;
    while (current_token) 
    {
        if (current_token->type == TOKEN_WORD || current_token->type == TOKEN_QUOTE || current_token->type == TOKEN_ENV_VAR || current_token->type == TOKEN_EXIT_STATUS) 
        {
             if (!current_cmd)
                initialize_new_command(&current_cmd, &head, &tail);
            if (current_token->type == TOKEN_ENV_VAR)
                handle_token_env_var(env_list, current_token, current_cmd);
            if (current_token->type == TOKEN_EXIT_STATUS)
                current_token->value = ft_itoa(e_status->last_exit_status);
            while (current_token->next && current_token->next->type == TOKEN_MERGE_FLAG && current_token->next->next)
            {
               // handle_token_merge_flag(current_token, e_status);
        	    char *merged_arg = NULL;
       			if (strcmp(current_token->value, "dontadd") != 0)
    				merged_arg = strdup(current_token->value);
    			else
        			merged_arg = strdup("");
                Token *new_token = current_token;
                current_token = current_token->next->next;
                if(current_token->type == TOKEN_EXIT_STATUS)
                    current_token->value = ft_itoa(e_status->last_exit_status);
        		else if (strcmp(current_token->value, "dontadd") == 0)
        			current_token->value = strdup("");
                char *temp = merged_arg;
                merged_arg = ft_strjoin(temp, current_token->value);
                free(temp);
                current_token->value = strdup(merged_arg);
                current_token->type = TOKEN_WORD; 
                free(merged_arg);
            }
            if (strcmp(current_token->value, "dontadd") != 0)
                add_argument(current_cmd, current_token->value);
        } 
        else if (current_token->type == TOKEN_PIPE) 
        {
            if (handle_token_pipe(&current_token, &current_cmd, &tail) == 1)
                return (NULL);
        } 
        else if (current_token->type == TOKEN_INPUT_REDIRECT)
        {
            if (handle_token_input_redirect(current_cmd, &current_token) == 1)
                return (NULL);
        } 
        else if (current_token->type == TOKEN_OUTPUT_REDIRECT || current_token->type == TOKEN_APPEND_REDIRECT) 
        {
            if (handle_token_output_append(current_cmd, &current_token) == 1)
                return (NULL);
        }
        else if (current_token->type == TOKEN_HEREDOC) 
            handle_token_heredoc(&current_token, &current_cmd, &head, &tail);
        current_token = current_token->next;
    }
    *tokens = current_token;
    return head;
}
