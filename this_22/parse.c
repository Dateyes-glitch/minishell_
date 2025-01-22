/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 20:56:35 by hawild            #+#    #+#             */
/*   Updated: 2025/01/22 21:38:24 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


void handle_token_env_var(t_envvar **env_list, t_token *current_token, t_command *current_cmd)
{
    char *env_value;

    env_value = get_env_var(env_list, current_token->value);
    if (env_value)
        current_token->value = ft_strdup(env_value);
    else
        current_token->value = ft_strdup("dontadd");
    if (!current_cmd->args && env_value)
        current_cmd->env_exp = ft_strdup(env_value);
    if (!env_value)
        current_cmd->env_exp = ft_strdup(":still:");
}

//  char *merged_arg = NULL;
//        			if (strcmp(current_token->value, "dontadd") != 0)
//     				merged_arg = ft_strdup(current_token->value);
//     			else
//         			merged_arg = ft_strdup("");
//                 Token *new_token = current_token;
//                 current_token = current_token->next->next;
//                 if(current_token->type == TOKEN_EXIT_STATUS)
//                     current_token->value = ft_itoa(e_status->last_exit_status);
//         		else if (strcmp(current_token->value, "dontadd") == 0)
//         			current_token->value = ft_strdup("");
//                 char *temp = merged_arg;
//                 merged_arg = ft_strjoin(temp, current_token->value);
//                 free(temp);
//                 current_token->value = ft_strdup(merged_arg);
//                 current_token->type = TOKEN_WORD; 
//                 free(merged_arg);

              
void handle_token_merge_flag(t_envvar **env_list, t_command *current_cmd, t_token *current_token, t_shell_status *e_status)
{
    char *merged_arg;
    char *temp;
    t_token *new_token;

    temp = NULL;
    merged_arg = NULL;
    if (strcmp(current_token->value, "dontadd") != 0)
        merged_arg = ft_strdup(current_token->value);
    else
    {
        merged_arg = ft_strdup("");
        current_token = current_token->next->next;
        if (current_token->type == TOKEN_EXIT_STATUS)
            current_token->value = ft_itoa(e_status->last_exit_status);
        if (current_token->type == TOKEN_ENV_VAR)
            handle_token_env_var(env_list, current_token, current_cmd);
        else if (strcmp(current_token->value, "dontadd") == 0)
            current_token->value = ft_strdup("");
        
        temp = merged_arg;
        merged_arg = ft_strjoin(temp,current_token->value);
        free(temp);
    }

    current_token->value = ft_strdup(merged_arg);
    current_token->type = TOKEN_WORD;
    free(merged_arg);
}

// void handle_token_merge_flag(Token **current_token, shell_status *e_status)
// {
//     char *merged_arg;
//     char *temp;
//     Token *tmp;

//     tmp = *current_token;
//     merged_arg = NULL;
//     if (strcmp((*current_token)->value, "dontadd") != 0)
//         merged_arg = ft_strdup(tmp->value);
//     else
//     {
//         merged_arg = ft_strdup("");
//         tmp = tmp->next->next;
//         if (tmp->type == TOKEN_EXIT_STATUS)
//             tmp->value = ft_itoa(e_status->last_exit_status);
//         else if (strcmp(tmp->value, "dontadd") == 0)
//             tmp->value = ft_strdup("");
//         temp = merged_arg;
//         merged_arg = ft_strjoin(temp, tmp->value);
//         free(temp);

//         tmp->value = ft_strdup(merged_arg);
//         tmp->type = TOKEN_WORD;
//         (*current_token) = tmp; 
//         free(merged_arg);
//     }
// }


int handle_token_input_redirect(t_command **current_cmd, t_token **current_token)
{
    if (!(*current_cmd)) 
    {
      (*current_cmd)= new_command();
     (*current_cmd)->exit_status = 2;
        printf("minishell: syntax error near unexpected token '<'\n");        
        return (1);
    }
    if((*current_cmd)->output_file!=NULL)
        (*current_cmd)->output = 1;
    *current_token = (*current_token)->next; // Update the pointer in the caller
    if (!(*current_token) || ((*current_token)->type != TOKEN_WORD && 
                               (*current_token)->type != TOKEN_QUOTE &&
                               (*current_token)->type != TOKEN_DOUBLE_QUOTE))
    {
        printf("minishell : syntax error near unexpected token '<'\n");
      ( *current_cmd)->exit_status = 2;
        return (1);
    }
   (*current_cmd)->input_file = ft_strdup((*current_token)->value);
    if (access((*current_cmd)->input_file, F_OK) == 0 && access((*current_cmd)->input_file,R_OK) != 0)
            (*current_cmd)->exit_status = 127;
    return (0);
}

int handle_token_output_append(t_command **current_cmd, t_token **current_token)
{
    int fd;

    if (!(*current_cmd)) 
    {
        (*current_cmd) = new_command();
        *current_token = (*current_token)->next;
        (*current_cmd)->output_file = ft_strdup((*current_token)->value);
        fd = open((*current_cmd)->output_file,O_WRONLY | O_CREAT | O_TRUNC, 0644);
        return (1);
    }
    if ((*current_cmd)->output_file)
        fd = open((*current_cmd)->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    (*current_cmd)->append = ((*current_token)->type == TOKEN_APPEND_REDIRECT);
    *current_token = (*current_token)->next;
    if (!(*current_token) || ((*current_token)->type != TOKEN_WORD &&
                              (*current_token)->type != TOKEN_QUOTE &&
                              (*current_token)->type != TOKEN_DOUBLE_QUOTE))
    {
        (*current_cmd) = new_command();
        (*current_cmd)->exit_status = 2;
        printf("minishell: syntax error near unexpected token '>' or '>>'\n");
        return 1;
    }
    (*current_cmd)->output_file = ft_strdup((*current_token)->value);
    if(access((*current_cmd)->output_file, F_OK) == 0 && access((*current_cmd)->output_file, W_OK) != 0)
            (*current_cmd)->exit_status = 300;
    return (0);
}



int handle_token_heredoc(t_token **current_token, t_command **current_cmd, t_command **head, t_command **tail)
{
    if (!(*current_cmd))
    {
        (*current_cmd) = new_command();
        if (!(*head))
            (*head) = (*current_cmd);
        if (*tail)
            (*tail)->next = *current_cmd;
        *tail = *current_cmd;
    }
    *current_token = (*current_token)->next;
    if (!(*current_token) || (*current_token)->type != TOKEN_WORD)
    {
        (*current_cmd)->exit_status = 2;
        printf("minishell: syntax error near unexpected token '<<'\n");
        return 1;
    }
    (*current_cmd)->heredoc_delim = ft_strdup((*current_token)->value);
    return 0;
}

int handle_token_pipe(t_token **current_token, t_command **current_cmd, t_command **tail)
{
    if (!(*current_cmd))
    {
        (*current_cmd) = new_command();
        (*current_cmd)->exit_status = 2;
        printf("minishell: syntax error\n");
        return 1;
    }
    // Ensure there is a next token; otherwise, it's a syntax error
    if (!(*current_token)->next) {
        printf("minishell: syntax cannot end with pipe\n");
		(*current_cmd)->exit_status = 2;
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

void initialize_new_command(t_command **current_cmd, t_command **head, t_command **tail)
{
    *current_cmd = new_command();
    if (!*head) 
        *head = *current_cmd;
    else 
        (*tail)->next = *current_cmd;
    *tail = *current_cmd;
}



// void process_token_content(Token **current_token, Command **current_cmd, Command **head, Command **tail, 
//                            envvar **env_list, shell_status *e_status)
// {
//     if (!*current_cmd) 
//         initialize_new_command(current_cmd, head, tail);

//     if ((*current_token)->type == TOKEN_ENV_VAR)
//         handle_token_env_var(env_list, *current_token, *current_cmd);

//     if ((*current_token)->type == TOKEN_EXIT_STATUS)
//         (*current_token)->value = ft_itoa(e_status->last_exit_status);

//     while ((*current_token)->next && (*current_token)->next->type == TOKEN_MERGE_FLAG && 
//            (*current_token)->next->next)
//         handle_token_merge_flag(*current_token, e_status);

//     if (strcmp((*current_token)->value, "dontadd") != 0)
//         add_argument(*current_cmd, (*current_token)->value);
// }
void the_loop(t_token *token, t_command *cmd, t_envvar **env_list, t_shell_status *e_status)
{
    t_token *current_token = token;
    t_command *current_cmd = cmd;

    while (current_token->next && current_token->next->type == TOKEN_MERGE_FLAG && current_token->next->next)
    {
        char *merged_arg = strdup(current_token->value);
        t_token *new_token = current_token;
        current_token = current_token->next->next; // Move to the token after the merge flag

        if (current_token->type == TOKEN_EXIT_STATUS)
        {
            // current_token->value = ft_itoa(e_status->last_exit_status);
            if (g_execute_external_running != 130 && g_execute_external_running != 131)
                current_token->value = ft_itoa(e_status->last_exit_status);
            else
            {
                if (g_execute_external_running == 130)
                {
                    current_token->value = ft_itoa(130);
                }
                else if (g_execute_external_running == 131)
                {
                    current_token->value = ft_itoa(131);
                }
                g_execute_external_running = 0;
            }
        }
        if (current_token->type == TOKEN_ENV_VAR)
        {
            handle_token_env_var(env_list, current_token, current_cmd);
        }

        // Concatenate the next token's value to merged_arg
        char *temp = merged_arg;
        merged_arg = ft_strjoin(temp, current_token->value);
        free(temp);

        // Update the current token with the merged value and change its type
        current_token->value = strdup(merged_arg);
        current_token->type = TOKEN_WORD;
        *token = *current_token;
        free(merged_arg);
    }
}


t_command *parse_pipeline(t_token **tokens, t_envvar **env_list, t_shell_status *e_status)
{
    t_command *head;
    t_command *tail;
    t_command *current_cmd;
    t_token *current_token;
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
            {
				if (g_execute_external_running != 130 && g_execute_external_running != 131)
                	current_token->value = ft_itoa(e_status->last_exit_status);
				else
				{
					if (g_execute_external_running == 130)
					{
						current_token->value = ft_itoa(130);
					}
					else if (g_execute_external_running == 131)
					{
						current_token->value = ft_itoa(131);
					}
					g_execute_external_running = 0;
				}
            }
            the_loop(current_token, current_cmd, env_list, e_status);
            if (strcmp(current_token->value, "dontadd") != 0)
                add_argument(current_cmd, current_token->value);
        } 
        else if (current_token->type == TOKEN_PIPE) 
        {
            if (handle_token_pipe(&current_token, &current_cmd, &tail) == 1)
                return (current_cmd);
        } 
        else if (current_token->type == TOKEN_INPUT_REDIRECT)
        {
            if(handle_token_input_redirect(&current_cmd, &current_token) == 1)
                return (current_cmd);
        } 
        else if (current_token->type == TOKEN_OUTPUT_REDIRECT || current_token->type == TOKEN_APPEND_REDIRECT) 
        {
            if (handle_token_output_append(&current_cmd, &current_token) == 1)
                return (current_cmd);
        }
        else if (current_token->type == TOKEN_HEREDOC)
        {
            if (handle_token_heredoc(&current_token, &current_cmd, &head, &tail) == 1)
                return (current_cmd);
        }
        current_token = current_token->next;
    }
    *tokens = current_token;
    return head;
}

// void handle_token_env_var(t_envvar **env_list, t_token *current_token, t_command *current_cmd)
// {
//     char *env_value;

//     env_value = get_env_var(env_list, current_token->value);
//     if (env_value)
//         current_token->value = ft_strdup(env_value);
//     else
//         current_token->value = ft_strdup("dontadd");
//     if (!current_cmd->args && env_value)
//         current_cmd->env_exp = ft_strdup(env_value);
//     if (!env_value)
//         current_cmd->env_exp = ft_strdup(":still:");
// }

//                 // char *merged_arg = strdup(current_token->value);
//                 // Token *new_token = current_token;
//                 // current_token = current_token->next->next; // Move to the token after the merge flag
               
//                 // if(current_token->type == TOKEN_EXIT_STATUS)
//                 //     current_token->value = ft_itoa(e_status->last_exit_status);
// 				// if (current_token->type == TOKEN_ENV_VAR)
// 				// {
// 				// 	char *env_value = get_env_var(env_list, current_token->value);
// 				// 	if (env_value)
// 				// 		current_token->value = strdup(env_value);
// 				// 	else
// 				// 		current_token->value = strdup("dontadd");
// 				// 	if (!current_cmd->args && env_value)
// 				// 		current_cmd->env_exp = strdup(env_value);
// 				// 	if (!env_value)
// 				// 		current_cmd->env_exp = strdup(":still:");
// 				// }
//                 // // Concatenate the next token's value to merged_arg
//                 // char *temp = merged_arg;
//                 // merged_arg = ft_strjoin(temp, current_token->value);
//                 // free(temp);

//                 // // Update the current token with the merged value and change its type
//                 // current_token->value = strdup(merged_arg);
//                 // current_token->type = TOKEN_WORD; 

//                 // free(merged_arg);

              
// void handle_token_merge_flag(t_envvar **env_list, t_command *current_cmd, t_token *current_token, t_shell_status *e_status)
// {
//     char *merged_arg;
//     t_token *new_token;
//     char *tmp;

//     merged_arg = strdup(current_token->value);
//     new_token = current_token;
//     current_token = current_token->next->next;
//     if (current_token->type == TOKEN_EXIT_STATUS)
//         current_token->value = ft_itoa(e_status->last_exit_status);
//     if (current_token->type == TOKEN_ENV_VAR)
//     {
//         char *env_value = get_env_var(env_list, current_token->value);
// 		if (env_value)
// 			current_token->value = strdup(env_value);
// 		else
// 			current_token->value = strdup("dontadd");
// 		if (!current_cmd->args && env_value)
// 			current_cmd->env_exp = strdup(env_value);
// 		if (!env_value)
// 			current_cmd->env_exp = strdup(":still:");
//     }
//     tmp = merged_arg;
//     merged_arg = ft_strjoin(tmp, current_token->value);
//     free(tmp);
//     current_token->value = ft_strdup(merged_arg);
//     current_token->type = TOKEN_WORD;
//     free(merged_arg);
// }

// // void handle_token_merge_flag(Token **current_token, shell_status *e_status)
// // {
// //     char *merged_arg;
// //     char *temp;
// //     Token *tmp;

// //     tmp = *current_token;
// //     merged_arg = NULL;
// //     if (strcmp((*current_token)->value, "dontadd") != 0)
// //         merged_arg = ft_strdup(tmp->value);
// //     else
// //     {
// //         merged_arg = ft_strdup("");
// //         tmp = tmp->next->next;
// //         if (tmp->type == TOKEN_EXIT_STATUS)
// //             tmp->value = ft_itoa(e_status->last_exit_status);
// //         else if (strcmp(tmp->value, "dontadd") == 0)
// //             tmp->value = ft_strdup("");
// //         temp = merged_arg;
// //         merged_arg = ft_strjoin(temp, tmp->value);
// //         free(temp);

// //         tmp->value = ft_strdup(merged_arg);
// //         tmp->type = TOKEN_WORD;
// //         (*current_token) = tmp; 
// //         free(merged_arg);
// //     }
// // }


// int handle_token_input_redirect(t_command *current_cmd, t_token **current_token)
// {
//     if (!current_cmd) 
//     {
//         perror("minishell: syntax error near unexpected token '<'");
//         return (1);
//     }
//     if (current_cmd->output_file != NULL)
//         current_cmd->output = 1;
//     *current_token = (*current_token)->next; // Update the pointer in the caller
//     if (!(*current_token) || ((*current_token)->type != TOKEN_WORD && 
//                                (*current_token)->type != TOKEN_QUOTE &&
//                                (*current_token)->type != TOKEN_DOUBLE_QUOTE))
//     {
//         perror("minishell|: syntax error near unexpected token '<'");
//         return (1);
//     }
//     current_cmd->input_file = ft_strdup((*current_token)->value);
//     calculate(current_cmd);
//     return (0);
// }

// int handle_token_output_append(t_command *current_cmd, t_token **current_token)
// {
//     int fd;

//     if (!current_cmd) 
//     {
//         perror("minishell: syntax error near unexpected token '>' or '>>'");
//         return (1);
//     }
//     if (current_cmd->output_file)
//         fd = open(current_cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
//     current_cmd->append = ((*current_token)->type == TOKEN_APPEND_REDIRECT);
//     *current_token = (*current_token)->next;
//     if (!(*current_token) || ((*current_token)->type != TOKEN_WORD &&
//                               (*current_token)->type != TOKEN_QUOTE &&
//                               (*current_token)->type != TOKEN_DOUBLE_QUOTE))
//     {
//         perror("minishell: syntax error near unexpected token '>' or '>>'");
//         return (1);
//     }
//     current_cmd->output_file = ft_strdup((*current_token)->value);
//     return (0);
// }



// void handle_token_heredoc(t_token **current_token, t_command **current_cmd, t_command **head, t_command **tail)
// {
//     if (!(*current_cmd))
//     {
//         *current_cmd = new_command();
//         if (!(*head))
//             *head = *current_cmd;
//         if (*tail)
//             (*tail)->next = *current_cmd;
//         *tail = *current_cmd;
//     }
//     *current_token = (*current_token)->next;
//     if (!(*current_token) || (*current_token)->type != TOKEN_WORD)
//     {
//         fprintf(stderr, "minishell: syntax error near unexpected token '<<'\n");
//         return;
//     }
//     (*current_cmd)->heredoc_delim = ft_strdup((*current_token)->value);
// }

// int handle_token_pipe(t_token **current_token, t_command **current_cmd, t_command **tail)
// {
//     // Ensure there is a next token; otherwise, it's a syntax error
//     if (!(*current_token)->next) {
//         fprintf(stderr, "minishell: syntax error near unexpected token '|'\n");
//         return 1;
//     }

//     // If there is an active command, finalize it and prepare for a new one
//     if (*current_cmd)
//     {
//         *tail = *current_cmd;
//         *current_cmd = NULL;
//     }

//     return 0;
// }

// void initialize_new_command(t_command **current_cmd, t_command **head, t_command **tail)
// {
//     *current_cmd = new_command();
//     if (!*head) 
//         *head = *current_cmd;
//     else 
//         (*tail)->next = *current_cmd;
//     *tail = *current_cmd;
// }



// // void process_token_content(Token **current_token, Command **current_cmd, Command **head, Command **tail, 
// //                            envvar **env_list, shell_status *e_status)
// // {
// //     if (!*current_cmd) 
// //         initialize_new_command(current_cmd, head, tail);

// //     if ((*current_token)->type == TOKEN_ENV_VAR)
// //         handle_token_env_var(env_list, *current_token, *current_cmd);

// //     if ((*current_token)->type == TOKEN_EXIT_STATUS)
// //         (*current_token)->value = ft_itoa(e_status->last_exit_status);

// //     while ((*current_token)->next && (*current_token)->next->type == TOKEN_MERGE_FLAG && 
// //            (*current_token)->next->next)
// //         handle_token_merge_flag(*current_token, e_status);

// //     if (strcmp((*current_token)->value, "dontadd") != 0)
// //         add_argument(*current_cmd, (*current_token)->value);
// // }


// // t_command *parse_pipeline(t_token **tokens, t_envvar **env_list, t_shell_status *e_status)
// // {
// //     t_command *head;
// //     t_command *tail;
// //     t_command *current_cmd;
// //     t_token *current_token;
// //     int flag;

// //     current_token = *tokens;
// //     head = NULL;
// //     tail = NULL;
// //     current_cmd = NULL;
// //     flag = 0;
// //     while (current_token) 
// //     {
// //         if (current_token->type == TOKEN_WORD || current_token->type == TOKEN_QUOTE || current_token->type == TOKEN_ENV_VAR || current_token->type == TOKEN_EXIT_STATUS) 
// //         {
// //           //  process_token_content(&current_token, &current_cmd, &head, &tail, env_list, e_status);
// //             if (!current_cmd)
// //                 initialize_new_command(&current_cmd, &head, &tail);
// //             if (current_token->type == TOKEN_ENV_VAR)
// //                 handle_token_env_var(env_list, current_token, current_cmd);
// //             if (current_token->type == TOKEN_EXIT_STATUS)
// //                 current_token->value = ft_itoa(e_status->last_exit_status);
// //             while (current_token->next && current_token->next->type == TOKEN_MERGE_FLAG && current_token->next->next)
// //             {
// //                 //handle_token_merge_flag(env_list, current_cmd, current_token, e_status);
// //                 char *merged_arg = strdup(current_token->value);
// //                 t_token *new_token = current_token;
// //                 current_token = current_token->next->next; // Move to the token after the merge flag
               
// //                 if(current_token->type == TOKEN_EXIT_STATUS)
// //                     current_token->value = ft_itoa(e_status->last_exit_status);
// // 				if (current_token->type == TOKEN_ENV_VAR)
// // 				{
// // 					char *env_value = get_env_var(env_list, current_token->value);
// // 					if (env_value)
// // 						current_token->value = strdup(env_value);
// // 					else
// // 						current_token->value = strdup("dontadd");
// // 					if (!current_cmd->args && env_value)
// // 						current_cmd->env_exp = strdup(env_value);
// // 					if (!env_value)
// // 						current_cmd->env_exp = strdup(":still:");
// // 				}
// //                 // Concatenate the next token's value to merged_arg
// //                 char *temp = merged_arg;
// //                 merged_arg = ft_strjoin(temp, current_token->value);
// //                 free(temp);

// //                 // Update the current token with the merged value and change its type
// //                 current_token->value = strdup(merged_arg);
// //                 current_token->type = TOKEN_WORD; 

// //                 free(merged_arg);
// //             }
// //                //handle_token_merge_flag(env_list, current_cmd, current_token, e_status);
// //             if (strcmp(current_token->value, "dontadd") != 0)
// //                 add_argument(current_cmd, current_token->value);
// //         } 
// //         else if (current_token->type == TOKEN_PIPE) 
// //         {
// //             if (handle_token_pipe(&current_token, &current_cmd, &tail) == 1)
// //                 return (NULL);
// //         } 
// //         else if (current_token->type == TOKEN_INPUT_REDIRECT)
// //         {
// //             if (handle_token_input_redirect(current_cmd, &current_token) == 1)
// //                 return (NULL);
// //         } 
// //         else if (current_token->type == TOKEN_OUTPUT_REDIRECT || current_token->type == TOKEN_APPEND_REDIRECT) 
// //         {
// //             if (handle_token_output_append(current_cmd, &current_token) == 1)
// //                 return (NULL);
// //         }
// //         else if (current_token->type == TOKEN_HEREDOC) 
// //             handle_token_heredoc(&current_token, &current_cmd, &head, &tail);
// //         current_token = current_token->next;
// //     }
// //     *tokens = current_token;
// //     return head;
// // }



// t_command *parse_pipeline(t_token **tokens, t_envvar **env_list, t_shell_status *e_status)
// {
//     t_command *head = NULL;
//     t_command *tail = NULL;
//     t_command *current_cmd = NULL;
//     int flag = 0;

//     t_token *current_token = *tokens;
//     while (current_token) 
//     {
//         if (current_token->type == TOKEN_WORD || current_token->type == TOKEN_QUOTE || current_token->type == TOKEN_ENV_VAR || current_token->type == TOKEN_EXIT_STATUS) 
//         {
//             // If there's no current command, start a new one
//             if (!current_cmd) 
//             {
//                 current_cmd = new_command();
//                 if (!head) 
//                     head = current_cmd;
//                 else 
//                     tail->next = current_cmd;
//                 tail = current_cmd;
//             }
//             if (current_token->type == TOKEN_ENV_VAR)
//             {
//                 char *env_value = get_env_var(env_list, current_token->value);
//                 if (env_value)
//                     current_token->value = strdup(env_value);
//                 else
//                     current_token->value = strdup("dontadd");
//                 if (!current_cmd->args && env_value)
//                     current_cmd->env_exp = strdup(env_value);
//                 if (!env_value)
//                     current_cmd->env_exp = strdup(":still:");
//             }
//             if (current_token->type == TOKEN_EXIT_STATUS)
//             {
// 				// current_token->value = ft_itoa(e_status->last_exit_status);
// 				if (g_execute_external_running != 130 && g_execute_external_running != 131)
//                 	current_token->value = ft_itoa(e_status->last_exit_status);
// 				else
// 				{
// 					if (g_execute_external_running == 130)
// 					{
// 						current_token->value = ft_itoa(130);
// 					}
// 					else if (g_execute_external_running == 131)
// 					{
// 						current_token->value = ft_itoa(131);
// 					}
// 					g_execute_external_running = 0;
// 				}
//             }
//             while (current_token->next && current_token->next->type == TOKEN_MERGE_FLAG && current_token->next->next) {
//                 char *merged_arg = strdup(current_token->value);
//                 t_token *new_token = current_token;
//                 current_token = current_token->next->next; // Move to the token after the merge flag
               
//                 if(current_token->type == TOKEN_EXIT_STATUS)
//                     current_token->value = ft_itoa(e_status->last_exit_status);
// 				if (current_token->type == TOKEN_ENV_VAR)
// 				{
// 					char *env_value = get_env_var(env_list, current_token->value);
// 					if (env_value)
// 						current_token->value = strdup(env_value);
// 					else
// 						current_token->value = strdup("dontadd");
// 					if (!current_cmd->args && env_value)
// 						current_cmd->env_exp = strdup(env_value);
// 					if (!env_value)
// 						current_cmd->env_exp = strdup(":still:");
// 				}
//                 // Concatenate the next token's value to merged_arg
//                 char *temp = merged_arg;
//                 merged_arg = ft_strjoin(temp, current_token->value);
//                 free(temp);

//                 // Update the current token with the merged value and change its type
//                 current_token->value = strdup(merged_arg);
//                 current_token->type = TOKEN_WORD; 

//                 free(merged_arg);
//             }
//             if (ft_strcmp(current_token->value, "dontadd") != 0)
//                 add_argument(current_cmd, current_token->value);
//         } 
//         else if (current_token->type == TOKEN_PIPE) 
//         {
// 			 if (!current_cmd) 
//             {
// 				current_cmd = new_command();
// 				current_cmd->exit_status = 2;
//                 printf("minishell: syntax error\n");
//                 return current_cmd;
//             }
//             if (!current_token->next)
//             {
//             	printf("minishell: syntax cannot end with pipe\n");
// 				current_cmd->exit_status = 2;
//                 return current_cmd;
//             }
//             if (current_cmd) 
//             {
//                 tail = current_cmd;
//                 current_cmd = NULL;
//             }
//         } 
//         else if (current_token->type == TOKEN_INPUT_REDIRECT) {
//             if (!current_cmd) 
//             {
//                 current_cmd = new_command();
// 				current_cmd->exit_status = 2;
//                 printf("minishell: syntax error near unexpected token '<'\n");
//                 return current_cmd;
//             }
//             if(current_cmd->output_file != NULL)
//                 current_cmd->output = 1;
//             current_token = current_token->next;
//             if (!current_token || (current_token->type != TOKEN_WORD && current_token->type != TOKEN_QUOTE && current_token->type != TOKEN_DOUBLE_QUOTE))
//             {
//                 printf("minishell : syntax error near unexpected token '<'\n");
//                 current_cmd->exit_status = 2;
// 				return current_cmd;
//             }
//             current_cmd->input_file = strdup(current_token->value);
//       			calculate(current_cmd);
//       			if (access(current_cmd->input_file, F_OK) == 0 && access(current_cmd->input_file,R_OK) != 0)
//       			{
//       					current_cmd->exit_status = 127;
//       					return current_cmd;
//       			}
//         } 
//         else if (current_token->type == TOKEN_OUTPUT_REDIRECT || current_token->type == TOKEN_APPEND_REDIRECT) 
//         {
//             int fd;
//             if (!current_cmd) 
//             {
// 				current_cmd = new_command();
// 				current_token = current_token->next;
// 				current_cmd->output_file = ft_strdup(current_token->value);
// 				fd = open(current_cmd->output_file,O_WRONLY | O_CREAT | O_TRUNC, 0644);
//                 // Ensure a current command exists
//                 return current_cmd;
//             }
//             if (current_cmd->output_file)
//                 fd = open(current_cmd->output_file,O_WRONLY | O_CREAT | O_TRUNC, 0644);
//             current_cmd->append = (current_token->type == TOKEN_APPEND_REDIRECT);
//             current_token = current_token->next;
//             if (!current_token || (current_token->type != TOKEN_WORD && current_token->type != TOKEN_QUOTE && current_token->type != TOKEN_DOUBLE_QUOTE))
//             {
// 				current_cmd = new_command();
// 				current_cmd->exit_status = 2;
//                 printf("minishell: syntax error near unexpected token '>' or '>>'\n");
//                 return current_cmd;
//             }
//             current_cmd->output_file = strdup(current_token->value);
//       			if(access(current_cmd->output_file, F_OK) == 0 && access(current_cmd->output_file, W_OK) != 0)
//       			{
//       					current_cmd->exit_status = 300;
//       					return current_cmd;
//       			}
//         }
//         else if (current_token->type == TOKEN_HEREDOC) 
//         {
//             if (!current_cmd) 
//             {
//                 current_cmd = new_command();
//                 if (!head) 
//                     head = current_cmd;
//                 if (tail) 
//                     tail->next = current_cmd;
//                 tail = current_cmd;
//             }
//             current_token = current_token->next;
//             if (!current_token || current_token->type != TOKEN_WORD) 
//             {
// 				current_cmd->exit_status = 2;
//                 printf("minishell: syntax error near unexpected token '<<'\n");
//                 return current_cmd;
//             }
//             current_cmd->heredoc_delim = strdup(current_token->value); 
//             // You'll handle the heredoc in execute_command_node
//         }
//         current_token = current_token->next;
//     }
//     *tokens = current_token;
//     return head;
// }