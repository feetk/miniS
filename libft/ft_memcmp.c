/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 19:22:00 by aganganu          #+#    #+#             */
/*   Updated: 2025/04/30 19:22:00 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*ps1;
	const unsigned char	*ps2;
	size_t				i;

	ps1 = (const unsigned char *)s1;
	ps2 = (const unsigned char *)s2;
	i = 0;
	while (i < n && ps1[i] == ps2[i])
		i++;
	if (i == n)
		return (0);
	return (ps1[i] - ps2[i]);
}
