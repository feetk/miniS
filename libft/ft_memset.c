/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:01:17 by aganganu          #+#    #+#             */
/*   Updated: 2025/05/03 21:23:18 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*r;
	size_t			i;

	i = 0;
	r = (unsigned char *)s;
	while (i < n)
	{
		r[i] = c;
		i++;
	}
	return (s);
}
