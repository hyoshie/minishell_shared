/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yshimazu <yshimazu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/01 21:56:57 by yshimazu          #+#    #+#             */
/*   Updated: 2021/12/13 20:47:39 by yshimazu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include "libft/mylibft.h"
# include <stdint.h>
# include <stdbool.h>
# include <string.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>

# define EQUAL 0

# define MAX_PROC 709

typedef enum e_stdfd
{
	SAVED_IN,
	SAVED_OUT,
	SAVED_ERR,
	STDFD_NUM
}	t_stdfd;

typedef struct	s_info
{
	t_dict	*env;
	char	**environ;
	int		stdfd[STDFD_NUM];
}	t_info;

//yoshie struct
enum e_kind
{
	WORD,
	HEREDOC,
	APPEND,
	IN_REDIRECT,
	OUT_REDIRECT,
	PIPE,
	LAST,
};

typedef struct s_token
{
	struct s_token	*prev;
	struct s_token	*next;
	char			*word;
	enum e_kind		kind;
}	t_token;

typedef struct s_io
{
	struct s_io	*prev;
	struct s_io	*next;
	char			*word;
	enum e_kind		kind;
}	t_io;

typedef struct s_proc
{
	struct s_proc	*prev;
	struct s_proc	*next;
	char			**cmd;
	t_io			*io_info;
	size_t			id;
}	t_proc;

//要検討
#define PATHNAME_SIZE 512

//utils.c
char	*ft_trijoin(char const *s1, char const *s2, char const *s3);
char	**ft_splitfree(char **tab);
int	ft_strcmp(const char *s1, const char *s2);
char	*mini_getenv(const char *key, t_info *info);
char	*ft_strajoin(char **str, char *connector);
char	*ft_strtok(char *str, char *sep);


//test.c 後で消す
void	test_args_check(char **args);
void	print_all_env(t_dict *env);

//shell_ecec
int	exec_shell(t_proc *proc, t_info *info);

//exec_builtin.c
int	exec_builtin(t_proc *proc, t_info *info);
bool	is_builtin(char **args);

//exec_echo.c
int	exec_echo(char **args, t_info *info);

//shell_init.c
t_info	*shell_init(void);

//exec_export.c
int	exec_export(char **args, t_info *info);

//env_utils.c
void	add_envs(char **envs, t_info *info);
void	del_envs(char **envs, t_info *info);

//exec_unset.c
int	exec_unset(char **args, t_info *info);

//dict_utils.c
bool	dict_delone(t_dict *elem);
bool	dict_addback(t_dict *head, t_dict *new);
t_dict	*dict_new(char *key, char *value);
void	dict_clear(t_dict *head);
size_t	dict_size(t_dict *head);

//error.c
void	cmd_err(char **cmd);

//loop_shell.c
void	loop_shell(t_info *info);
char	**shell_split_line(char *line);
int	shell_launch(t_proc *proc, t_info *info);

//bool.c
bool	is_single_proc(t_proc *proc);
bool	is_redirect(t_proc *proc);
bool	is_first_proc(t_proc *proc);
bool	is_last_proc(t_proc *proc);

//xutils.c
void	xperror(char *s);
void	*xmalloc(size_t size);
void	xpipe(int *fd);
pid_t	xfork(void);
void	xdup2(int fd1, int fd2);

//exec_shell.c
void	redirect_pipe(t_io *io_info, t_info *info);
void	save_stdfd(t_info *info);


//exec_builtin.c
void	redirect_reset(t_info *info);

////////parser///////
int		parse_line(t_proc **proclist, char *line);
t_proc	*to_proclist(t_token *tokens);
bool	validate_syntax(t_token *tokens);
size_t	count_quote(char *word);
t_token	*remove_quote(t_token *tokens);
char	*remove_quote_in_token(char *word);
size_t	count_process(t_token *tokens);
t_proc	*init_procinfo(size_t proc_num);
t_proc	*set_procinfo(t_proc *procs, t_token *tokens, size_t proc_num);
t_io	*set_ioinfo(t_token *tokens);
char	**set_cmdinfo(t_token **tokens);
bool	is_io_operator(enum e_kind kind);
bool	is_pipe(enum e_kind kind);
bool	is_op_kind(enum e_kind kind);
bool	is_cmd(t_token *token);
//list
t_proc	*proc_lstnew(void);
t_proc	*proc_lstlast(t_proc *lst);
void	proc_lstadd_back(t_proc **lst, t_proc *new);
void	proc_lstdelone(t_proc *lst);
void	proc_lstclear(t_proc **lst);
t_io	*io_lstnew(void);
t_io	*io_lstlast(t_io *lst);
void	io_lstadd_back(t_io **lst, t_io *new);
void	io_lstdelone(t_io *lst, void (*del)(void *));
void	io_lstclear(t_io **lst, void(*del)(void *));



//////lexer///////
t_token	*to_tokenlist(char *line);
size_t	tkn_strlen(const char *start);
bool	is_ifs(char c);
bool	is_operator(char c);
bool	is_2chr_operator(const char *str);
bool	is_quote(char c);
bool	is_end(char c);
char	*ft_strndup(const char *s1, size_t n);
t_token	*tkn_lstnew(char *str);
t_token	*tkn_lstlast(t_token *lst);
void	tkn_lstadd_back(t_token **lst, t_token *new);

//////others///////
//test
void	test_pipe(t_info *info);

//to_token.c
t_token	*to_tokenlist(char *line);

#endif