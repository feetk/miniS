/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 15:37:40 by aganganu          #+#    #+#             */
/*   Updated: 2026/01/18 15:10:03 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	token_add_back(t_token **tok, t_token *new)
{
	t_token	*last;

	if (!new)
		return ;
	if (!*tok)
	{
		*tok = new;
		return ;
	}
	last = *tok;
	while(last->next)
		last = last->next;
	last->next = new;
}

t_token	*token_new(char *word, t_token_type type)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	new->type = type;
	new->word = word;
	new->next = NULL;
	return (new);
}

int	handle_op(char *line, t_token **tok, int i)
{
	int				len;
	t_token_type	type;

	len = 0;
	type = 0;
	if (line[i] == '|')
	{
		len = 1;
		type = TOKEN_PIPE;
	}
	else if (line[i] == '<')
	{
		if (line[i + 1] == '<')
		{
			type = TOKEN_HEREDOC;
			len = 2;
		}
		else
		{
			len = 1;
			type = TOKEN_REDIR_IN;
		}
	}
	else if (line[i] == '>')
	{
		if (line[i + 1] == '>')
		{
			type = TOKEN_APPEND;
			len = 2;
		}
		else
		{
			len = 1;
			type = TOKEN_REDIR_OUT;
		}
	}
	token_add_back(tok, token_new(ft_substr(line, i, len), type));
	return (len);
}

int	is_whitespaces(char c)
{
	return ((c == ' ' || c == '\t' || c == '\n' || c == '\v')
		|| (c == '\f' || c == '\r'));
}

t_token	*lexer(char *line)
{
	t_token	*tok;
	int		i;
	int		verif;

	tok = NULL;
	verif = 0;
	i = 0;
	while (line[i])
	{
		if (is_whitespaces(line[i]))
		{
			i++;
			continue;
		}
		if (is_op(line[i]))
		{
			verif = handle_op(line, &tok, i);
			if (verif == -1)
				return (free_tok(tok), NULL);
			i += verif;
		}
		else
		{
			verif = handle_word(line, &tok, i);
			if (verif == -1)
				return (free_tok(tok), NULL);
			i += verif;
		}
	}
	return (tok);
}
