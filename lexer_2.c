/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 18:00:55 by aganganu          #+#    #+#             */
/*   Updated: 2026/01/18 15:33:59 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_op(char c)
{
	return (c == '|' || c == '>' || c == '<');
}

int	get_word_len(int i, char *line)
{
	int	len;

	len = 0;
	while (line[i + len] && !is_whitespaces(line[i + len]) && !is_op(line[i + len]))
	{
		if (line[i + len] == '\'')
		{
			len++;
			while (line[i + len] != '\'' && line[i + len])
				len++;
			if (line[i + len] == '\0')
				return (-1);
			len++;
		}
		else if (line[i + len] && line[i + len] == '\"')
		{
			len++;
			while (line[i + len] != '\"' && line[i + len])
				len++;
			if (line[i + len] == '\0')
				return (-1);
			len++;
		}
		else
			len++;
	}
	return (len);
}

int	handle_word(char *line, t_token **tok, int i)
{
	int		len;
	char	*word;

	len = get_word_len(i, line);
	if (len == -1)
		return (-1);
	word = ft_substr(line, i, len);
	if (!word)
		return (-1);
	token_add_back(tok, token_new(word, TOKEN_WORD));
	return (len);
}

void	free_tok(t_token *tok)
{
	t_token	*temp;

	while (tok)
	{
		temp = tok->next;
		if (tok->word)
			free(tok->word);
		free(tok);
		tok = temp;
	}
}
