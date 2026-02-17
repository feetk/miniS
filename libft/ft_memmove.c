/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 18:58:41 by aganganu          #+#    #+#             */
/*   Updated: 2026/01/11 14:55:09 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	long				i;

	if (!dest && !src)
		return (NULL);
	i = 0;
	if (dest < src)
	{
		while ((size_t)i < n)
		{
			*(unsigned char *)(dest + i) = *(unsigned char *)(src + i);
			i++;
		}
		return (dest);
	}
	else if (dest > src)
	{
		i = n - 1;
		while (i >= 0)
		{
			*(unsigned char *)(dest + i) = *(unsigned char *)(src + i);
			i--;
		}
	}
	return (dest);
}
