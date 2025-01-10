
// #include "minishell.h"

// void    part_one(Command *current_cmd,Token *current_token, envvar **env_list, shell_status *e_status)
// {
//     if (current_token->type == TOKEN_ENV_VAR)
//     {
//         char *env_value = get_env_var(env_list, current_token->value);
//         if (env_value)
//             current_token->value = strdup(env_value);
//         else
//             current_token->value = strdup("dontadd");
//         if (!current_cmd->args && env_value)
//             current_cmd->env_exp = strdup(env_value);
//         if (!env_value)
//             current_cmd->env_exp = strdup(":still:");
//     }
//     if (current_token->type == TOKEN_EXIT_STATUS)
//     {
//         current_token->value = ft_itoa(e_status->last_exit_status);
//     }
// }

// void    part_two(Command *current_cmd,Token *current_token, envvar **env_list, shell_status *e_status)
// {
//     while (current_token->next && current_token->next->type == TOKEN_MERGE_FLAG && current_token->next->next) 
//     {
//         char *merged_arg = strdup(current_token->value);
//         Token *new_token = current_token;
//         current_token = current_token->next->next; // Move to the token after the merge flag
        
//         if(current_token->type == TOKEN_EXIT_STATUS)
//             current_token->value = ft_itoa(e_status->last_exit_status);
//         // Concatenate the next token's value to merged_arg
//         char *temp = merged_arg;
//         merged_arg = ft_strjoin(temp, current_token->value);
//         free(temp);

//         // Update the current token with the merged value and change its type
//         current_token->value = strdup(merged_arg);
//         current_token->type = TOKEN_WORD; 

//         free(merged_arg);
//     }
//     if (strcmp(current_token->value, "dontadd") != 0)
//         add_argument(current_cmd, current_token->value);

// }

// Command *parse_pipeline(Token **tokens, envvar **env_list, shell_status *e_status)
// {
//     Command *head = NULL;
//     Command *tail = NULL;
//     Command *current_cmd = NULL;
//     int flag = 0;

//     Token *current_token = *tokens;
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
//             part_one(current_cmd, current_token, env_list, e_status);
//             part_two(current_cmd, current_token, env_list, e_status);
//         } 
//         else if (current_token->type == TOKEN_PIPE) 
//         {
//             if (!current_token->next)
//             {
//                 perror("minishell: syntax cannot end with pipe");
//                 return NULL;
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
//                 // Ensure a current command exists
//                 perror("minishell: syntax error near unexpected token '<'");
//                 return NULL;
//             }
//             if(current_cmd->output_file != NULL)
//                 current_cmd->output = 1;
//             current_token = current_token->next;
//             if (!current_token || (current_token->type != TOKEN_WORD && current_token->type != TOKEN_QUOTE && current_token->type != TOKEN_DOUBLE_QUOTE))
//             {
//                 perror("minishell: syntax error near unexpected token '<'");
//                 return NULL;
//             }
//             current_cmd->input_file = strdup(current_token->value);
//         } 
//         else if (current_token->type == TOKEN_OUTPUT_REDIRECT || current_token->type == TOKEN_APPEND_REDIRECT) 
//         {
//             int fd;
//             if (!current_cmd) 
//             {
//                 // Ensure a current command exists
//                 perror("minishell: syntax error near unexpected token '>' or '>>'");
//                 return NULL;
//             }
//             if (current_cmd->output_file)
//                 fd = open(current_cmd->output_file,O_WRONLY | O_CREAT | O_TRUNC, 0644);
//             current_cmd->append = (current_token->type == TOKEN_APPEND_REDIRECT);
//             current_token = current_token->next;
//             if (!current_token || (current_token->type != TOKEN_WORD && current_token->type != TOKEN_QUOTE && current_token->type != TOKEN_DOUBLE_QUOTE))
//             {
//                 perror("minishell: syntax error near unexpected token '>' or '>>'");
//                 return NULL;
//             }
//             current_cmd->output_file = strdup(current_token->value);
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
//                 fprintf(stderr, "minishell: syntax error near unexpected token '<<'\n");
//                 free_commands(head);
//                 return NULL;
//             }
//             current_cmd->heredoc_delim = strdup(current_token->value); 
//             // You'll handle the heredoc in execute_command_node
//         }
//         current_token = current_token->next;
//     }

//     *tokens = current_token;
//     return head;
// }