#include "minishell.h"

char	*ft_strjoin(char *str1, char *str2)
{
	char	*str_cp1;
	char	*str_cp2;
	char	*joined;
	int		i;
	int		j;

	str_cp1 = (char *)str1;
	str_cp2 = (char *)str2;
	joined = (char *)malloc(sizeof(char) * (strlen(str_cp1) + strlen(str_cp2)
				+ 1));
	i = 0;
	j = 0;
	while (str_cp1[i])
	{
		joined[i] = str_cp1[i];
		i++;
	}
	while (str_cp2[j])
	{
		joined[i + j] = str_cp2[j];
		j++;
	}
	joined[i + j] = '\0';
	return ((char *)joined);
}

static int	ft_get_len(int n)
{
	int	len;

	len = 0;
	if (n <= 0)
		len++;
	while (n != 0)
	{
		n = n / 10;
		len++;
	}
	return (len);
}

static char	*ft_number_to_str(char *str, int n, int len, int start)
{
	while (len > start)
	{
		str[len - 1] = n % 10 + '0';
		n = n / 10;
		len--;
	}
	return (str);
}

char	*ft_itoa(int n)
{
	int		start;
	int		len;
	char	*str;

	start = 0;
	len = ft_get_len(n);
	str = (char *)malloc(sizeof(char) * (len + 1));
	if (!str)
		return (0);
	if (n == -2147483648)
	{
		str[0] = '-';
		str[1] = '2';
		n = 147483648;
		start = 2;
	}
	if (n < 0)
	{
		str[0] = '-';
		n = -n;
		start = 1;
	}
	ft_number_to_str(str, n, len, start);
	str[len] = '\0';
	return (str);
}

Command	*new_command(void)
{
	Command	*cmd;

	cmd = malloc(sizeof(Command));
	if (!cmd)
	{
		perror("minishell: malloc");
		exit(EXIT_FAILURE);
	}
	cmd->args = NULL;
	cmd->input_file = NULL;
	cmd->output_file = NULL;
	cmd->append = 0;
	cmd->exit_status = 0;
	cmd->next = NULL;
	cmd->heredoc = 0;
	cmd->heredoc_delim = NULL;
	cmd->env_exp = NULL;
	cmd->output = 0;
	return (cmd);
}

void	add_argument(Command *cmd, char *arg)
{
	int	count;

	count = 0;
	while (cmd->args && cmd->args[count])
		count++;
	cmd->args = realloc(cmd->args, (count + 2) * sizeof(char *));
	if (!cmd->args)
	{
		perror("minishell: realloc");
		exit(EXIT_FAILURE);
	}
	cmd->args[count] = strdup(arg);
	cmd->args[count + 1] = NULL;
}

void	add_arguments(Command *cmd, char **arguments)
{
	int count;
	int i;
	int start;

	count = 0;
	while (cmd->args && cmd->args[count])
		count++;
	start = count;
	i = 0;
	while (arguments[i])
	{
		i++;
		count++;
	}
	cmd->args = realloc(cmd->args, (count + 1) * sizeof(char *));
	if (!cmd->args)
	{
		perror("minishell: malloc");
		exit(EXIT_FAILURE);
	}
	i = 0;
	while (start != count)
		cmd->args[start++] = strdup(arguments[i++]);
	cmd->args[start] = NULL;
}

void calculate(Command *cmd)
{
	int calc = 1000;
	int fd = 0;
	if (cmd->input_file || cmd->output_file)
	{
		calc = check_permissions(cmd->input_file);
		if (calc == 5)
			cmd->exit_status = 127;
		else if (calc != 0 && cmd->args == NULL)
			cmd->exit_status = 126;
		else if (calc != 0 && cmd->args != NULL)
			cmd->exit_status = 1;
		else
			cmd->exit_status = 0;
	}
}

Command *parse_pipeline(Token **tokens, envvar **env_list, shell_status *e_status)
{
    Command *head = NULL;
    Command *tail = NULL;
    Command *current_cmd = NULL;
    int flag = 0;

    Token *current_token = *tokens;
    while (current_token) 
    {
        if (current_token->type == TOKEN_WORD || current_token->type == TOKEN_QUOTE || current_token->type == TOKEN_ENV_VAR || current_token->type == TOKEN_EXIT_STATUS) 
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
                    current_token->value = strdup("dontadd");
                if (!current_cmd->args && env_value)
                    current_cmd->env_exp = strdup(env_value);
                if (!env_value)
                    current_cmd->env_exp = strdup(":still:");
            }
            if (current_token->type == TOKEN_EXIT_STATUS)
            {
				// current_token->value = ft_itoa(e_status->last_exit_status);
				if (execute_external_running != 130 && execute_external_running != 131)
                	current_token->value = ft_itoa(e_status->last_exit_status);
				else
				{
					if (execute_external_running == 130)
					{
						current_token->value = ft_itoa(130);
					}
					else if (execute_external_running == 131)
					{
						current_token->value = ft_itoa(131);
					}
					execute_external_running = 0;
				}
            }
            while (current_token->next && current_token->next->type == TOKEN_MERGE_FLAG && current_token->next->next) {
                char *merged_arg = strdup(current_token->value);
                Token *new_token = current_token;
                current_token = current_token->next->next; // Move to the token after the merge flag
               
                if(current_token->type == TOKEN_EXIT_STATUS)
                    current_token->value = ft_itoa(e_status->last_exit_status);
				if (current_token->type == TOKEN_ENV_VAR)
				{
					char *env_value = get_env_var(env_list, current_token->value);
					if (env_value)
						current_token->value = strdup(env_value);
					else
						current_token->value = strdup("dontadd");
					if (!current_cmd->args && env_value)
						current_cmd->env_exp = strdup(env_value);
					if (!env_value)
						current_cmd->env_exp = strdup(":still:");
				}
                // Concatenate the next token's value to merged_arg
                char *temp = merged_arg;
                merged_arg = ft_strjoin(temp, current_token->value);
                free(temp);

                // Update the current token with the merged value and change its type
                current_token->value = strdup(merged_arg);
                current_token->type = TOKEN_WORD; 

                free(merged_arg);
            }
            if (strcmp(current_token->value, "dontadd") != 0)
                add_argument(current_cmd, current_token->value);
        } 
        else if (current_token->type == TOKEN_PIPE) 
        {
			 if (!current_cmd) 
            {
				current_cmd = new_command();
				current_cmd->exit_status = 2;
                printf("minishell: syntax error\n");
                return current_cmd;
            }
            if (!current_token->next)
            {
            	printf("minishell: syntax cannot end with pipe\n");
				current_cmd->exit_status = 2;
                return current_cmd;
            }
            if (current_cmd) 
            {
                tail = current_cmd;
                current_cmd = NULL;
            }
        } 
        else if (current_token->type == TOKEN_INPUT_REDIRECT) {
            if (!current_cmd) 
            {
                current_cmd = new_command();
				current_cmd->exit_status = 2;
                printf("minishell: syntax error near unexpected token '<'\n");
                return current_cmd;
            }
            if(current_cmd->output_file != NULL)
                current_cmd->output = 1;
            current_token = current_token->next;
            if (!current_token || (current_token->type != TOKEN_WORD && current_token->type != TOKEN_QUOTE && current_token->type != TOKEN_DOUBLE_QUOTE))
            {
                printf("minishell : syntax error near unexpected token '<'\n");
                current_cmd->exit_status = 2;
				return current_cmd;
            }
            current_cmd->input_file = strdup(current_token->value);
      			calculate(current_cmd);
      			if (access(current_cmd->input_file, F_OK) == 0 && access(current_cmd->input_file,R_OK) != 0)
      			{
      					current_cmd->exit_status = 127;
      					return current_cmd;
      			}
        } 
        else if (current_token->type == TOKEN_OUTPUT_REDIRECT || current_token->type == TOKEN_APPEND_REDIRECT) 
        {
            int fd;
            if (!current_cmd) 
            {
				current_cmd = new_command();
				current_token = current_token->next;
				current_cmd->output_file = ft_strdup(current_token->value);
				fd = open(current_cmd->output_file,O_WRONLY | O_CREAT | O_TRUNC, 0644);
                // Ensure a current command exists
                return current_cmd;
            }
            if (current_cmd->output_file)
                fd = open(current_cmd->output_file,O_WRONLY | O_CREAT | O_TRUNC, 0644);
            current_cmd->append = (current_token->type == TOKEN_APPEND_REDIRECT);
            current_token = current_token->next;
            if (!current_token || (current_token->type != TOKEN_WORD && current_token->type != TOKEN_QUOTE && current_token->type != TOKEN_DOUBLE_QUOTE))
            {
				current_cmd = new_command();
				current_cmd->exit_status = 2;
                printf("minishell: syntax error near unexpected token '>' or '>>'\n");
                return current_cmd;
            }
            current_cmd->output_file = strdup(current_token->value);
      			if(access(current_cmd->output_file, F_OK) == 0 && access(current_cmd->output_file, W_OK) != 0)
      			{
      					current_cmd->exit_status = 300;
      					return current_cmd;
      			}
        }
        else if (current_token->type == TOKEN_HEREDOC) 
        {
            if (!current_cmd) 
            {
                current_cmd = new_command();
                if (!head) 
                    head = current_cmd;
                if (tail) 
                    tail->next = current_cmd;
                tail = current_cmd;
            }
            current_token = current_token->next;
            if (!current_token || current_token->type != TOKEN_WORD) 
            {
				current_cmd->exit_status = 2;
                printf("minishell: syntax error near unexpected token '<<'\n");
                return current_cmd;
            }
            current_cmd->heredoc_delim = strdup(current_token->value); 
            // You'll handle the heredoc in execute_command_node
        }
        current_token = current_token->next;
    }
    *tokens = current_token;
    return head;
}

void	free_commands(Command *commands)
{
	Command *current;
	Command *next;
	int i;

	current = commands;
	while (current)
	{
		next = current->next;
		if (current->args)
		{
			i = 0;
			while (current->args[i])
			{
				free(current->args[i]);
				i++;
			}
			free(current->args);
		}
		if (current->input_file)
			free(current->input_file);
		if (current->output_file)
			free(current->output_file);
		current = next;
	}
}
