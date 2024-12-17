#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ft_strlen(char *string)
{
    int i = 0;
    while(string[i])
        i++;
    return i;
}

char *ft_process_double_quotes(char *input)
{
    int i = 0;
    int j = ft_strlen(input) - 1;
    char *str = NULL;
    while(input[i] == '"'&& input[j] == '"' && i < j)
    {
        i++;
        j--;
    }
    if ((input[i] == '"' || input[j] == '"') && i!=j)
    {
        printf("Unclosed double quotes");
        return NULL;
    }
    else
    {
        if (i % 2 != 0)
        {
            int len = j - i + 1;
            str = (char *)malloc(len + 1);
            if (str!=NULL)
            {
                strncpy(str, &input[i], len);
                str[len] = '\0';
            }
            return (str);
        }
        else
        {
            while(input[i] == '\'' && input[j] == '\'' && i < j)
            {
                i++;
                j--;
            }
            if ((input[i] == '\'' && input[j] == '\'') && i != j)
            {
                printf("Unclosed single quotes");
                return NULL;
            }
            else
            {
                int len = j - i + 1;
                str = (char *)malloc(len + 1);
                if (str != NULL)
                {
                    strncpy(str, &input[i], len);
                    str[len] = '\0';
                }
                return (str);
            }
        }
    }
}


char *ft_process_single_quotes(char *input)
{
    int i = 0;
    int j = ft_strlen(input) - 1;
    char *str = NULL;
    while(input[i] == '\''&& input[j] == '\'' && i < j)
    {
        i++;
        j--;
    }
    if ((input[i] == '\'' || input[j] == '\'') && i!=j)
    {
        printf("Unclosed double quotes");
        return NULL;
    }
    else
    {
        if (i % 2 != 0)
        {
            int len = j - i + 1;
            str = (char *)malloc(len + 1);
            if (str!=NULL)
            {
                strncpy(str, &input[i], len);
                str[len] = '\0';
            }
            return (str);
        }
        else
        {
            while(input[i] == '"' && input[j] == '"' && i < j)
            {
                i++;
                j--;
            }
            if ((input[i] == '"' && input[j] == '"') && i != j)
            {
                printf("Unclosed single quotes");
                return NULL;
            }
            else
            {
                int len = j - i + 1;
                str = (char *)malloc(len + 1);
                if (str != NULL)
                {
                    strncpy(str, &input[i], len);
                    str[len] = '\0';
                }
                return (str);
            }
        }
    }
}

int main()
{
    char input[256];
    printf("Enter a string: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';
    char *result = NULL;
    int i = 0;

    while (input[i])
    {
        if (input[i] == '"')
        {
            result = ft_process_double_quotes(input);
            printf("iteration: %i, result: %s\n", i, result);
            i++;
        }
        else if (input[i] == '\'')
        {
            result = ft_process_single_quotes(input);
            printf("iteration: %i, result: %s\n", i, result);
            i++;
        }
        else
            break;
    }
    // if (input[0] == '"')
    //     result = ft_process_double_quotes(input);
    // else if (input[0] == '\'')
    //     result = ft_process_single_quotes(input);
    // else
    // {
    //     printf("Extracted substring: %s\n", input);
    //     return 0;
    // }        
    //char *result = ft_process_quotes(input);
    if (result != NULL)
    {
        printf("Extracted substring:\n %s\n", result);
        free(result);
    }
    return 0;
}

