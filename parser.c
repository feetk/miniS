/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 13:27:05 by aganganu          #+#    #+#             */
/*   Updated: 2026/02/17 15:15:44 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_cmd	*cmd_new(void)
{
	t_cmd	*new;

	new = malloc(sizeof(t_cmd));
	if (!new)
		return (NULL);
	new->args = NULL;
	new->next = NULL;
	new->redirs = NULL;
	return (new);
}

void	cmd_add_back(t_cmd **cmd_list, t_cmd *new_cmd)
{
	t_cmd	*temp;

	if (!new_cmd || !cmd_list)
		return ;
	if (!*cmd_list)
	{
		*cmd_list = new_cmd;
		return ;
	}
	temp = *cmd_list;
	while (temp->next)
		temp = temp->next;
	temp->next = new_cmd;
}

int	is_redir(t_token_type type)
{
	return ((type == TOKEN_APPEND || type == TOKEN_HEREDOC)
		|| (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT));
}

int	count_args(t_token *tok)
{
	int			count;
	t_token		*temp;

	count = 0;
	temp = tok;
	while (temp && temp->type != TOKEN_PIPE)
	{
		if (is_redir(temp->type))
		{
			temp = temp->next;
			if (temp)
				temp = temp->next;
		}
		else
		{
			count++;
			temp = temp->next;
		}
	}
	return (count);
}

t_redir *redir_new(t_token_type type, char *file)
{
	t_redir	*new;

	new = malloc(sizeof(t_redir));
	if (!new)
		return (NULL);
	new->type = type;
	new->file = ft_strdup(file);
	new->next = NULL;
	return (new);
}

void	redir_add_back(t_redir **redir_list, t_redir *new_redir)
{
	t_redir	*temp;

	if (!redir_list || !new_redir)
		return ;
	if (!*redir_list)
	{
		*redir_list = new_redir;
		return ;
	}
	temp = *redir_list;
	while (temp->next)
		temp = temp->next;
	temp->next = new_redir;	
}

void	add_redir(t_cmd *cmd, t_token **tok_ptr)
{
	t_redir	*new;

	if (!(*tok_ptr)->next || (*tok_ptr)->next->type != TOKEN_WORD)
	{
		*tok_ptr = (*tok_ptr)->next;
		return ;
	}
	new = redir_new((*tok_ptr)->type, (*tok_ptr)->next->word);
	redir_add_back(&(cmd->redirs), new);
	*tok_ptr = (*tok_ptr)->next->next;
}

t_cmd	*parse_tokens(t_token *tok)
{
	t_cmd	*cmd_list;
	t_cmd	*current_cmd;
	int		i;

	if (!tok)
		return (NULL);
	i = 0;
	cmd_list = NULL;
	current_cmd = NULL;
	current_cmd = cmd_new();
	if (!current_cmd)
		return (NULL);
	cmd_add_back(&cmd_list, current_cmd);
	current_cmd->args = malloc(sizeof(char *) * (count_args(tok) + 1));
	if (!current_cmd->args)
		return (NULL);
	while (tok)
	{
		if (tok->type == TOKEN_PIPE)
		{
			current_cmd->args[i] = 0;
			current_cmd = cmd_new();
			cmd_add_back(&cmd_list, current_cmd);
			current_cmd->args = malloc(sizeof(char *) * (count_args(tok->next) + 1));
			if (!current_cmd->args)
				return (NULL);
			i = 0;
			tok = tok->next;
		}
		else if (is_redir(tok->type))
			add_redir(current_cmd, &tok);
		else
		{
			current_cmd->args[i] = ft_strdup(tok->word);
			i++;
			tok = tok->next;
		}
	}
	current_cmd->args[i] = 0;
	return (cmd_list);
}
