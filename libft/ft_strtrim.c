/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 17:51:42 by aganganu          #+#    #+#             */
/*   Updated: 2025/04/29 17:51:42 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	isin(char const c, char const *set)
{
	int	i;

	i = 0;
	while (set[i])
	{
		if (set[i] == c)
			return (1);
		i++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*res;
	int		start;
	int		end;

	start = 0;
	end = ft_strlen(s1);
	while (start < ft_strlen(s1) && isin(s1[start], set))
		start++;
	while (end > start && isin(s1[end - 1], set))
		end--;
	res = malloc((end - start + 1) * sizeof(char));
	if (!res)
		return (NULL);
	ft_strlcpy(res, s1 + start, end - start + 1);
	return (res);
}
