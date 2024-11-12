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

char *ft_process_quotes(char *input)
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

int main()
{
    char input[256];
    printf("Enter a string: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    char *result = ft_process_quotes(input);
    if (result != NULL)
    {
        printf("Extracted substring:\n %s\n", result);
        free(result);
    }
    return 0;
}