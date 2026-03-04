/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsb <dsb@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 23:29:31 by dsb               #+#    #+#             */
/*   Updated: 2026/02/18 23:41:33 by dsb              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	heredoc_child(int *fd, char *delim)
{
	char	*line;

	close(fd[0]);
	setup_signals_heredoc();
	line = readline("> ");
	while (line)
	{
		if (ft_strncmp(line, delim, ft_strlen(delim) + 1) == 0)
			return (free(line), close(fd[1]), (void)exit(0));
		write(fd[1], line, ft_strlen(line));
		write(fd[1], "\n", 1);
		free(line);
		line = readline("> ");
	}
	close(fd[1]);
	exit(0);
}

int	heredoc_fd(char *delimiter)
{
	int		fd[2];
	pid_t	pid;

	if (pipe(fd) == -1)
		return (-1);
	pid = fork();
	if (pid == 0)
		heredoc_child(fd, delimiter);
	close(fd[1]);
	waitpid(pid, NULL, 0);
	return (fd[0]);
}
