/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 18:34:09 by aganganu          #+#    #+#             */
/*   Updated: 2025/05/01 16:55:15 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	size_t	i;

	i = 0;
	if (size == 0)
		return (ft_strlen((char *)src));
	while (src[i] && i + 1 < size)
	{
		dest[i] = src[i];
		i++;
	}
	if (size > 0)
	{
		dest[i] = '\0';
		i++;
	}
	return (ft_strlen((char *)src));
}
