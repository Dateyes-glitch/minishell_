#include "minishell.h"

char *ft_strjoin(char *str1,char *str2)
{
    char *str_cp1 = (char *)str1;
    char *str_cp2 = (char *)str2;
    char *joined = (char *)malloc(sizeof(char) * (strlen(str_cp1) + strlen(str_cp2) + 1));
    int i = 0;
    int j = 0;
    while(str_cp1[i])
    {
        joined[i] = str_cp1[i];
        i++;
    }
    while(str_cp2[j])
    {
        joined[i + j] = str_cp2[j];
        j++;
    }
    joined[i + j] = '\0';
    return (char *)joined;

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

static char *ft_strncpy(char *s1, char *s2, int n)
{
	int i = -1;

	while (++i < n && s2[i])
		s1[i] = s2[i];
	s1[i] = '\0';
	return (s1);
}

char	**ft_split(char *str)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int wc = 0;
	
	while (str[i])
	{
		while (str[i] && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'))
			i++;
		if (str[i])
			wc++;
		while (str[i] && (str[i] != ' ' && str[i] != '\t' && str[i] != '\n'))
			i++;
	}
	
	char **out = (char **)malloc(sizeof(char *) * (wc + 1));
	i = 0;
	
	while (str[i])
	{
		while (str[i] && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'))
			i++;
		j = i;
		while (str[i] && (str[i] != ' ' && str[i] != '\t' && str[i] != '\n'))
			i++;
		if (i > j)
		{
			out[k] = (char *)malloc(sizeof(char) * ((i - j) + 1));
			ft_strncpy(out[k++], &str[j], i - j);
		}
	}
	out[k] = NULL;
	return (out);
}

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
    cmd->exit_status = 0;
    cmd->next = NULL;
    cmd->heredoc = 0;
    cmd->heredoc_delim = NULL;
    cmd->env_exp = NULL;
    cmd->output = 0;
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

void add_arguments(Command *cmd, char **arguments)
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
    {
        cmd->args[start] = strdup(arguments[i]);
        i++;
        start++;
    }
    cmd->args[start] = NULL;
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
                    current_token->value = strdup("");
                if (!current_cmd->args)
                    current_cmd->env_exp = strdup(env_value);
            }
            if (current_token->type == TOKEN_EXIT_STATUS)
            {
                current_token->value = ft_itoa(e_status->last_exit_status);
            }
            while (current_token->next && current_token->next->type == TOKEN_MERGE_FLAG) {
                char *merged_arg = strdup(current_token->value);
                current_token = current_token->next->next; // Move to the token after the merge flag
               
                if(current_token->type == TOKEN_EXIT_STATUS)
                    current_token->value = ft_itoa(e_status->last_exit_status);
                // Concatenate the next token's value to merged_arg
                char *temp = merged_arg;
                merged_arg = ft_strjoin(temp, current_token->value);
                free(temp);

                // Update the current token with the merged value and change its type
                current_token->value = strdup(merged_arg);
                current_token->type = TOKEN_WORD; 

                free(merged_arg);
            }
            add_argument(current_cmd, current_token->value);
        } 
        else if (current_token->type == TOKEN_PIPE) 
        {
            if (!current_token->next)
            {
                perror("minishell: syntax cannot end with pipe");
                return NULL;
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
                // Ensure a current command exists
                perror("minishell: syntax error near unexpected token '<'");
                return NULL;
            }
            if(current_cmd->output_file != NULL)
                current_cmd->output = 1;
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
            int fd;
            if (!current_cmd) 
            {
                // Ensure a current command exists
                perror("minishell: syntax error near unexpected token '>' or '>>'");
                return NULL;
            }
            if (current_cmd->output_file)
                fd = open(current_cmd->output_file,O_WRONLY | O_CREAT | O_TRUNC, 0644);
            current_cmd->append = (current_token->type == TOKEN_APPEND_REDIRECT);
            current_token = current_token->next;
            if (!current_token || (current_token->type != TOKEN_WORD && current_token->type != TOKEN_QUOTE && current_token->type != TOKEN_DOUBLE_QUOTE))
            {
                perror("minishell: syntax error near unexpected token '>' or '>>'");
                return NULL;
            }
            current_cmd->output_file = strdup(current_token->value);
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
                fprintf(stderr, "minishell: syntax error near unexpected token '<<'\n");
                free_commands(head);
                return NULL;
            }
            current_cmd->heredoc_delim = strdup(current_token->value); 
            // You'll handle the heredoc in execute_command_node
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
