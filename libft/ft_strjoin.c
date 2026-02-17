/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 20:08:18 by aganganu          #+#    #+#             */
/*   Updated: 2025/04/29 17:50:57 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*res;
	int		len;
	int		len2;
	int		i;

	i = 0;
	len = ft_strlen(s1);
	len2 = ft_strlen(s2);
	res = malloc((len + len2 + 1) * sizeof(char));
	if (!res)
		return (NULL);
	while (s1[i])
	{
		res[i] = s1[i];
		i++;
	}
	i = 0;
	while (s2[i])
	{
		res[len + i] = s2[i];
		i++;
	}
	res[len + len2] = '\0';
	return (res);
}
