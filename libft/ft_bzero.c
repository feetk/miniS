/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/26 13:38:04 by aganganu          #+#    #+#             */
/*   Updated: 2025/05/01 14:36:51 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_bzero(void *s, size_t n)
{
	unsigned char	*r;
	size_t			i;

	i = 0;
	r = (unsigned char *)s;
	while (i < n)
	{
		r[i] = 0;
		i++;
	}
}
