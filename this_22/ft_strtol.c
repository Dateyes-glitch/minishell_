/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtol.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawild <hawild@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 20:57:50 by hawild            #+#    #+#             */
/*   Updated: 2025/01/22 20:59:44 by hawild           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	determine_base(const char **s, int base)
{
	if (base == 0)
	{
		if (**s == '0')
		{
			if ((*s)[1] == 'x' || (*s)[1] == 'X')
			{
				base = 16;
				(*s) += 2;
			}
			else
			{
				base = 8;
				(*s)++;
			}
		}
		else
			base = 10;
	}
	return (base);
}

int	process_digit(char c, int base, int *digit)
{
	if (ft_isdigit((unsigned char)c))
		*digit = c - '0';
	else if (ft_isalpha((unsigned char)c))
		*digit = ft_tolower((unsigned char)c) - 'a' + 10;
	else
		return (0);
	return (*digit < base);
}

int	handle_digit_and_overflow(long *result, int digit, int base,
		long sign, const char **s, char **endptr)
{
	if (*result > (LONG_MAX - digit) / base)
	{
		errno = ERANGE;
		if (sign == 1)
			*result = LONG_MAX;
		else
			*result = LONG_MIN;
		if (endptr)
			*endptr = (char *)*s;
		return (1);
	}
	*result = (*result) * base + digit;
	return (0);
}

long	parse_digits(const char **s, int base, long sign, char **endptr)
{
	long		result;
	const char	*start;
	int			digit;

	result = 0;
	start = *s;
	while (**s)
	{
		if (!process_digit(**s, base, &digit))
			break ;
		if (handle_digit_and_overflow(&result, digit, base, sign, s, endptr))
			return (result);
		(*s)++;
	}
	if (*s == start)
	{
		errno = EINVAL;
		if (endptr)
			*endptr = (char *)start;
		return (0);
	}
	if (endptr)
		*endptr = (char *)*s;
	return (result);
}

long	ft_strtol(const char *nptr, char **endptr, int base)
{
	const char	*s;
	long		result;
	int			sign;

	s = nptr;
	result = 0;
	sign = 1;
	s = skip_whitespace(s);
	sign = handle_sign(&s);
	if (base < 0 || base == 1 || base > 36)
	{
		if (endptr)
			*endptr = (char *)nptr;
		errno = EINVAL;
		return (0);
	}
	base = determine_base(&s, base);
	result = parse_digits(&s, base, sign, endptr);
	return (result * sign);
}