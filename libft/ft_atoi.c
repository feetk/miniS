/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 23:42:37 by aganganu          #+#    #+#             */
/*   Updated: 2025/04/27 23:42:37 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_space(char c)
{
	if ((c == ' ' || c == '\t' || c == '\v')
		|| (c == '\f' || c == '\n' || c == '\r'))
		return (1);
	return (0);
}

int	ft_atoi(const char *nptr)
{
	int	res;
	int	i;
	int	sign;

	sign = 1;
	res = 0;
	i = 0;
	while (ft_space(nptr[i]))
		i++;
	if (nptr[i] == '+' || nptr[i] == '-')
	{
		if (nptr[i] == '-')
			sign = -1;
		i++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		res = res * 10;
		res = res + nptr[i] - '0';
		i++;
	}
	res = res * sign;
	return (res);
}
