/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 18:24:54 by aganganu          #+#    #+#             */
/*   Updated: 2025/04/29 18:24:54 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_words(char const *str, char charset)
{
	int	i;
	int	res;
	int	state;

	i = 0;
	res = 0;
	state = 0;
	while (str[i])
	{
		if (charset != str[i])
		{
			if (!state)
			{
				res++;
				state = 1;
			}
		}
		else
			state = 0;
		i++;
	}
	return (res);
}

static int	strlen_set(char const *str, char charset, int i)
{
	int	res;

	res = 0;
	while (charset == str[i])
		i++;
	while (str[i] && charset != str[i])
	{
		res++;
		i++;
	}
	return (res);
}

static char	*build_word(char const *str, char charset, int i)
{
	char	*res;
	int		word_len;
	int		j;

	word_len = strlen_set(str, charset, i);
	res = malloc((word_len + 1) * sizeof(char));
	if (!res)
		return (0);
	j = 0;
	while (charset == str[i])
		i++;
	while (str[i] && charset != str[i])
	{
		res[j] = str[i];
		i++;
		j++;
	}
	res[j] = '\0';
	return (res);
}

static char	**ft_split_nv(char const *s, char c, char **res, int i)
{
	int	state;
	int	j;

	state = 0;
	j = 0;
	while (s[i])
	{
		if ((c != s[i]) && !state)
		{
			res[j] = build_word(s, c, i);
			if (!res[j])
			{
				while (j >= 0)
					free(res[j--]);
				return (free(res), NULL);
			}
			state = (j++, 1);
		}
		else if (c == s[i])
			state = 0;
		i++;
	}
	return (res);
}

char	**ft_split(char const *s, char c)
{
	int		nb_words;
	char	**result;

	nb_words = count_words(s, c);
	result = malloc((nb_words + 1) * sizeof(char *));
	if (!result)
		return (0);
	result[nb_words] = 0;
	return (ft_split_nv(s, c, result, 0));
}
