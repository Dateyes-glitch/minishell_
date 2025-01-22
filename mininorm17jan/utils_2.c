/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 20:58:13 by hawild            #+#    #+#             */
/*   Updated: 2025/01/17 18:55:09 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

char	*ft_substr(const char *s, unsigned int start, size_t len)
{
	char			*ret;
	unsigned int	i;

	if (!s)
		return (0);
	i = strlen(s);
	if (i < start)
	{
		return (ft_strdup(""));
	}
	else if (strlen(s + start) < len && len != 0)
		len = strlen(s + start);
	ret = malloc(sizeof(char) * (len + 1));
	if (!ret)
		return (0);
	ft_strlcpy(ret, s + start, len + 1);
	return (ret);
}
