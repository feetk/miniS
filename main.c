/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 15:10:31 by aganganu          #+#    #+#             */
/*   Updated: 2026/02/17 16:38:32 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

const char *get_token_type(int type)
{
    if (type == TOKEN_WORD)
        return ("WORD");
    else if (type == TOKEN_PIPE)
        return ("PIPE (|)");
    else if (type == TOKEN_REDIR_IN)
        return ("INPUT (<)");
    else if (type == TOKEN_REDIR_OUT)
        return ("OUTPUT (>)");
    else if (type == TOKEN_HEREDOC)
        return ("HEREDOC (<<)");
    else if (type == TOKEN_APPEND)
        return ("APPEND (>>)");
    return ("UNKNOWN");
}

int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	t_token	*tok;
	char	*line;
    t_cmd   *cmds;
    t_env   *env_list;

    env_list = init_env(env);
	while (1)
	{
		line = readline("minishell$> ");
        if (!line)
        {
            printf("exit\n");
            break;
        }
		if (line[0] != '\0')
       	{
        	add_history(line);
        	tok = lexer(line);
            if (tok)
            {
                expand(tok, env_list);
                remove_quotes(tok);
                cmds = parse_tokens(tok);
                print_cmds(cmds);
                free_cmds(cmds);
                free_tok(tok);
            }
        }
		free(line);
	}
    return (0);
}
