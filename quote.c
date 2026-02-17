/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 14:35:24 by aganganu          #+#    #+#             */
/*   Updated: 2026/02/17 16:04:24 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	get_clean_len(char *str)
{
	int	sq;
	int	dq;
	int	i;
	int	len;

	sq = 0;
	dq = 0;
	i = 0;
	len = 0;
	while (str[i])
	{
		if (str[i] == '\'' && !dq)
			sq = !sq;
		else if (str[i] == '\"' && !sq)
			dq = !dq;
		else
			len++;
		i++;
	}
	return (len);
}

void	clean_quotes(char **word_ptr)
{
	int		i;
	int		insq;
	int		indq;
	char	*word;
	int		j;

	i = 0;
	insq = 0;
	indq = 0;
	j = 0;
	word = malloc(sizeof(char) * (get_clean_len(*word_ptr) + 1));
	if (!word)
		return ;
	while ((*word_ptr)[i])
	{
		if ((*word_ptr)[i] == '\'' && !indq)
			insq = !insq;
		else if ((*word_ptr)[i] == '\"' && !insq)
			indq = !indq;
		else
		{
			word[j] = (*word_ptr)[i];
			j++;
		}
		i++;
	}
	word[j] = '\0';
	free(*word_ptr);
	*word_ptr = word;
}

void	remove_quotes(t_token *tok)
{
	while (tok)
	{
		if (tok->type == TOKEN_WORD)
			clean_quotes(&(tok->word));
		tok = tok->next;
	}
}
