/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsb <dsb@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 12:39:19 by aganganu          #+#    #+#             */
/*   Updated: 2026/02/19 03:37:07 by dsb              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <signal.h>
# include <sys/stat.h>
# include <string.h>
# include <errno.h>
# include <stdbool.h>
# include <readline/history.h>
# include <readline/readline.h>

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_APPEND,
	TOKEN_HEREDOC
}		t_token_type;

typedef struct s_token
{
	char			*word;
	t_token_type	type;
	struct s_token	*next;
}		t_token;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}		t_env;

typedef struct s_redir
{
	t_token_type	type;
	char			*file;
	struct s_redir	*next;
}		t_redir;

typedef struct s_cmd
{
	char			**args;
	t_redir			*redirs;
	struct s_cmd	*next;
}		t_cmd;

t_token	*token_new(char *word, t_token_type type);
void	token_add_back(t_token **tok, t_token *new);
t_token	*lexer(char *line);
int		is_whitespaces(char c);
int		handle_op(char *line, t_token **tok, int i);
int		is_op(char c);
int		get_word_len(int i, char *line);
int		handle_word(char *line, t_token **tok, int i);
void	free_tok(t_token *tok);
t_env	*init_env(char **env_array);
t_env	*env_new(char *key, char *value);
void	env_add_back(t_env **env, t_env *new);
char	*get_env_value(char *key, t_env *env);
t_env	*init_minimal_env(void);
int		is_valid_var(char c);
int		get_var_len(char *str);
int		get_new_len(char *str, int len_var_name, int len_value);
char	*create_s(char *old_str, char *value, int start, int len_var_name);
void	replace_var(char **str_ptr, int *index, t_env *env);
void	expand(t_token *tok, t_env *env);
int		get_clean_len(char *str);
void	clean_quotes(char **word_ptr);
void	remove_quotes(t_token *tok);
t_cmd	*cmd_new(void);
void	cmd_add_back(t_cmd **cmd_list, t_cmd *new_cmd);
int		count_args(t_token *tok);
int		is_redir(t_token_type type);
t_redir	*redir_new(t_token_type type, char *file);
void	redir_add_back(t_redir **redir_list, t_redir *new_redir);
void	add_redir(t_cmd *cmd, t_token **tok_ptr);
t_cmd	*parse_tokens(t_token *tok);
void	print_cmds(t_cmd *cmd);
void	free_cmds(t_cmd *cmd);

typedef struct s_pctx
{
	int	**pipes;
	int	n;
	int	i;
}		t_pctx;

extern int	g_signal;
extern int	g_last_exit;

void	handle_sigint(int sig);
void	setup_signals_interactive(void);
void	setup_signals_child(void);
void	setup_signals_heredoc(void);

char	*env_to_str(t_env *node);
char	**build_env(t_env *env);
char	*find_path(char *cmd, t_env *env);
void	free_env(t_env *env);
int		apply_redirs(t_redir *redir);
int		builtin_with_redir(t_cmd *cmd, t_env **env);

int		heredoc_fd(char *delimiter);

void	handle_heredocs(t_cmd *cmd, t_env **env);
void	exec_cmd(t_cmd *cmd, t_env **env);
int		exec_single(t_cmd *cmd, t_env **env);
int		exec_pipeline(t_cmd *cmds, t_env **env);
int		**alloc_pipes(int n);
void	close_pipes(int **pipes, int n);
int		execute(t_cmd *cmds, t_env **env);

int		is_builtin(char *cmd);
int		run_builtin(t_cmd *cmd, t_env **env);
int		ft_echo(t_cmd *cmd);
int		ft_pwd(void);
int		ft_env(t_env *env);
int		ft_exit(t_cmd *cmd);
int		ft_cd(t_cmd *cmd, t_env **env);
int		ft_unset(t_cmd *cmd, t_env **env);
int		ft_export(t_cmd *cmd, t_env **env);
int		export_one(char *arg, t_env **env);
void	print_export(t_env *env);
void	update_env(t_env *env, char *key, char *val);
void	env_set(t_env **env, char *key, char *val);

#endif