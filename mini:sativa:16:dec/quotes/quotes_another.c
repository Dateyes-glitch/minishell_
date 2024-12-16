# include <unistd.h>
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
// must return a char * 
// try to find the other double quote allocate memory for the "-----"
// return it 
//have a general function that will do that for single and double quotes
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



char *ft_double_quotes(char *inp)
{
    static char *ret = NULL;
    char *swt = NULL;
    int i = 1;
    int j = 1;
    int k = 0;
    while(inp[i] != '"' && inp[i] != '\0')
    {
        i++;
    }
    if (inp[i] == '\0')
    {
        printf("Unclosed double quote");
        return NULL;
    }
    ret = (char *)malloc(i * sizeof(char));
    if (i == 1)
    {
        if (inp[2] == '\0')
            return inp;
        swt = &inp[i+1];
        return swt;
    }
    while(inp[j] != '"' && inp[j] != '\0')
    {
        ret[k] = inp[j];
        k++;
        j++;
    }
    //probably store in a static variable for example resstat = strdupret
    swt = ft_strjoin(ret, &inp[i]);
    return swt;
}

int handle_quotes(char *inp)
{
    int j = 0;
    int i = 0;
    char *another = strdup(inp);
    while(another[i])
    {
        if (another[i] == '"')
            j++;
        i++;
    }
    return j;
}

char *handle_double(char *inp)
{
    static int j = 0;
    int i = 0;
    char *ret = strdup(inp);
    if (j == 0)
    {
        j = handle_quotes(ret);
        if(j % 2 != 0)
        {
            printf("Unclosed double quotes");
            return NULL;
        }
    }
    while(ret[i] && ret[i] == '"')
        i++;
    return &ret[i];
}

char *ft_word(char *str, char *extra)
{
    static int i = 0;
    int j = 0;
    char *ret = NULL;
    char *str_dup = strdup(str);
    while(str_dup[j] != '"' && str_dup[j] != '\0')
        j++;
    // i = i + j;
    ret = (char *)realloc(extra, (i + j) * sizeof(char));
    if (ret == NULL)
    {
        perror("realloc failed");
        return NULL;
    }
    j = 0;
    while(str_dup[j] != '"' && str_dup[j] != '\0')
    {
        ret[i] = str[j];
        i++;
        j++;
    }
    ret[i] = '\0';
    // free(str);
    return ret;
}
/*
int main()
{
    char input[256];
    char *result = NULL;
    char *extra = NULL;
    printf("Enter a string: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';
    result = strdup(input);
    int i = 0;
    // result = f
    while(result[i])
    {
        if (result[i]== '"')
        {
            result=handle_double(&result[i]);
            i = 0;
            if (result == NULL)
                break;
            //printf("%s\n", result);
        }
        else
        {
            if (extra == NULL)
                extra = strdup(&result[i]);
            ft_word(&result[i], extra);
            
            int k = i;
            while (result[k] != '"' && result[k] != '\0')
                k++;
            i = k;
            
        }
    }
    //free(result);
    if (result == NULL)
        printf("\n");
    else
        printf("result : %s\n", extra);
    return 0;
}


*/
