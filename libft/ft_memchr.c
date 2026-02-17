/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 19:38:39 by aganganu          #+#    #+#             */
/*   Updated: 2025/04/30 19:38:39 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*ps;
	size_t				i;

	i = 0;
	ps = (const unsigned char *)s;
	while (i < n)
	{
		if ((unsigned char)c == ps[i])
			return ((unsigned char *)ps + i);
		i++;
	}
	return (0);
}
