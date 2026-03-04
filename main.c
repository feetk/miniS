/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsb <dsb@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 15:10:31 by aganganu          #+#    #+#             */
/*   Updated: 2026/02/19 03:37:07 by dsb              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_last_exit = 0;

static void	handle_signal_after(void)
{
	if (g_signal)
	{
		g_last_exit = 128 + g_signal;
		g_signal = 0;
	}
}

static void	run_line(char *line, t_env **env)
{
	t_token	*tok;
	t_cmd	*cmds;

	add_history(line);
	tok = lexer(line);
	if (!tok)
		return ;
	expand(tok, *env);
	remove_quotes(tok);
	cmds = parse_tokens(tok);
	if (cmds)
	{
		g_last_exit = execute(cmds, env);
		free_cmds(cmds);
	}
	free_tok(tok);
}

int	main(int ac, char **av, char **env)
{
	t_env	*env_list;
	char	*line;

	(void)ac;
	(void)av;
	env_list = init_env(env);
	while (1)
	{
		setup_signals_interactive();
		line = readline("minishell$> ");
		if (!line)
		{
			write(1, "exit\n", 5);
			break ;
		}
		handle_signal_after();
		if (line[0] != '\0')
			run_line(line, &env_list);
		free(line);
	}
	free_env(env_list);
	rl_clear_history();
	return (g_last_exit);
}
