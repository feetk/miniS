/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:40:39 by aganganu          #+#    #+#             */
/*   Updated: 2026/02/17 16:32:35 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_valid_var(char c)
{
	return (ft_isalnum(c) || c == '_');
}

int	get_var_len(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '?')
		return (1);
	while (str[i] && is_valid_var(str[i]))
		i++;
	return (i);
}

int	get_new_len(char *str, int len_var_name, int len_value)
{
	int	len;

	len = 0;
	len = (ft_strlen(str) - (len_var_name + 1));
	len += len_value;
	return (len);
}

char	*create_s(char *old_str, char *value, int start, int len_var_name)
{
	char	*new_string;
	int		i;
	int		j;
	int		k;
	int		new_len;

	i = 0;
	j = 0;
	new_len = get_new_len(old_str, len_var_name, ft_strlen(value));
	new_string = malloc(sizeof(char) * (new_len + 1));
	if (!new_string)
		return (NULL);
	while (i < start)
	{
		new_string[i] = old_str[i];
		i++;
	}
	k = i;
	while (value[j])
		new_string[k++] = value[j++];
	i += len_var_name + 1;
	while (old_str[i])
		new_string[k++] = old_str[i++];
	new_string[k] = '\0';
	return (new_string);
}

void	replace_var(char **str_ptr, int *index, t_env *env)
{
	int		len_var_name;
	char	*var_name;
	char	*value;
	char	*new_str;
	char	*exit_str;

	len_var_name = get_var_len(*str_ptr + (*index + 1));
	var_name = ft_substr(*str_ptr, *index + 1, len_var_name);
	exit_str = NULL;
	if (var_name && var_name[0] == '?' && var_name[1] == '\0')
	{
		exit_str = ft_itoa(g_last_exit);
		value = exit_str;
	}
	else
		value = get_env_value(var_name, env);
	free(var_name);
	new_str = create_s(*str_ptr, value, *index, len_var_name);
	free(*str_ptr);
	*str_ptr = new_str;
	*index += ft_strlen(value) - 1;
	free(exit_str);
}

void	expand(t_token *tok, t_env *env)
{
	int		i;
	int		in_sq;
	int		in_dq;

	while (tok)
	{
		if (tok->type == TOKEN_WORD)
		{
			i = 0;
			in_dq = 0;
			in_sq = 0;
			while (tok->word[i])
			{
				if (tok->word[i] == '\'' && !in_dq)
					in_sq = !in_sq;
				else if (tok->word[i] == '\"' && !in_sq)
					in_dq = !in_dq;
				else if (tok->word[i] == '$' && !in_sq)
				{
					if (tok->word[i + 1] && (is_valid_var(tok->word[i + 1])
							|| tok->word[i + 1] == '?'))
						replace_var(&(tok->word), &i, env);
				}
				i++;
			}
		}
		tok = tok->next;
	}
}
