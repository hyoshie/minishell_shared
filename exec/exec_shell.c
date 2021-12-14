/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_shell.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yshimazu <yshimazu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/26 21:29:05 by yshimazu          #+#    #+#             */
/*   Updated: 2021/12/09 16:44:48 by yshimazu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

//move to yoshie's file
int	proc_num(t_proc *head)
{
	int	num;
	t_proc	*p;

	if (!head)
		return (0);
	num = 0;
	p = head;
	while (p)
	{
		p = p->next;
		num++;
	}
	return (num);
}

char	*path_from_env(char *cmd, char *strenv)
{
	int		i;
	char	*path;
	char	*ret;
	char	**path_each;

	i = 0;
	path = strenv + 5;
	path_each = ft_split(path, ':');
	i = -1;
	while (path_each[++i])
	{
		ret = ft_strdup(ft_trijoin(path_each[i], "/", cmd));
		if (access(ret, F_OK) == 0)
		{
			ft_splitfree(path_each);
			return (ret);
		}
	}
	ft_splitfree(path_each);
	return (0);
}

char	*get_path(char *cmd, char **sp_cmd, t_info *info)
{
	char		*strenv;

	(void)info;
	strenv = mini_getenv("PATH", info);
	if (strenv == NULL)
		xperror("getenv");
	if (access(sp_cmd[0], X_OK) == 0)
		return (sp_cmd[0]);
	else if (ft_strchr(sp_cmd[0], '/'))
	{
		//free(path);
		xperror(ft_trijoin("minishell", ": ", cmd));
		return (0);
	}
	else
		return (path_from_env(sp_cmd[0], strenv));
}

int	ft_open(char *file, enum e_kind open_mode)
{
	int	fd;

	if (open_mode == IN_REDIRECT)
	{
		fd = open(file, O_RDONLY);
		if (fd == -1)
			perror(ft_trijoin("minishell", ": ", file));
		return (fd);
	}
	else if (open_mode == OUT_REDIRECT)
	{
		fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (fd == -1)
			perror(ft_trijoin("minishell", ": ", file));
		return (fd);
	}
	else if (open_mode == APPEND)
	{
		fd = open(file, O_WRONLY | O_APPEND, 0666);
		if (fd == -1)
			fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (fd == -1)
			perror(ft_trijoin("minishell", ": ", file));
		return (fd);
	}	
	else
		return (0);
}

//change to with connecter
/* char	**dict_to_array(t_dict *dict_head)
{
	char	**array;
	t_dict	*p;
	size_t	i;
	size_t size;

	size = dict_size(dict_head);
	p = dict_head->next;
	array = xmalloc(sizeof(char *) * (size + 1));
	i = -1;
	while (++i < size)
	{
		array[i] = ft_trijoin(p->key, "=", p->value);
		p = p->next;
	}
	array[i] = NULL;
	return (array);
} */

//todo: need to fix environ in execve -> done I think...
int	ft_exec(char **cmd, t_info *info)
{
	char	*path;
	/* extern  */char	**environ;

	//sp_cmd = ft_split(cmd, ' ');
	environ = dict_to_array(info->env);
	path = get_path(cmd[0], cmd, info);
	execve(path, cmd, environ);
	free (path);
	cmd_err(cmd);
	return (0);
}

void	pipes_close(int pipes[][2], int num_pipes)
{
	int	i;
	
	i = 0;
	while (i < num_pipes)
	{
		//printf("i: %d\n", i);
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

int	xdup(int fd)
{
	int	ret;

	ret = dup(fd);
	if (ret == -1)
		xperror("dup2");
	return (ret);
}

//change to dup
void	save_stdfd(t_info *info)//change to stdfd
{
	info->stdfd[SAVED_IN] = xdup(STDIN_FILENO);
	info->stdfd[SAVED_OUT] = xdup(STDOUT_FILENO);
	info->stdfd[SAVED_ERR] = xdup(STDERR_FILENO);
}

void	close_stdfd(void)
{
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int	heredoc_open(char **heredoc_file_name)
{
	int	fd;
	char *name;
	char *i_str;
	int	i;

	name = ".heredoc";
	i = 0;
	fd = -1;	
	while (fd == -1)
	{
		if (*heredoc_file_name)
			free(*heredoc_file_name);
		i_str = ft_itoa(i);
		*heredoc_file_name = ft_strjoin(name, i_str);
		fd =  open(*heredoc_file_name, O_WRONLY | O_EXCL | O_CREAT, 0666);
		free(i_str);
		i++;
	}
	return (fd);
}

void	heredoc_handler(t_io *io_info, t_info *info)
{
	int fd;
	char *line;
	char *heredoc_file_name;

	heredoc_file_name = NULL;
	redirect_reset(info);
	fd = heredoc_open(&heredoc_file_name);
	while (1)
	{
		line = readline("> ");
		if (ft_strcmp(line, io_info->word) == 0)
			break;
		ft_putendl_fd(line, fd);
		free(line);
	}
	close(fd);//need to close somewhere
	fd = ft_open(heredoc_file_name, IN_REDIRECT);
	xdup2(fd, STDIN_FILENO);
	unlink(heredoc_file_name);
	free(heredoc_file_name);
	close(fd);
}

void	redirect_pipe(t_io *io_info, t_info *info)
{
	int	fd;

	(void)info; // use it later
	while(io_info)
	{
		if (io_info->kind == OUT_REDIRECT)
		{
			fd = ft_open(io_info->word, OUT_REDIRECT);
			xdup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (io_info->kind == IN_REDIRECT)
		{
			fd = ft_open(io_info->word, IN_REDIRECT);
			xdup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (io_info->kind == APPEND)
		{
			fd = ft_open(io_info->word, APPEND);
			xdup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (io_info->kind == HEREDOC)
		{
			heredoc_handler(io_info, info);		
		}
		io_info = io_info->next;
	}
	/* close_stdfd(); */
}

int	child_proc(t_proc *proc, int pipes[][2], t_info *info)
{
	//(void)info;//will use later maybe..
	save_stdfd(info);
	if (!is_first_proc(proc))
		xdup2(pipes[proc->id - 1][0], STDIN_FILENO);
	if (!is_last_proc(proc))
		xdup2(pipes[proc->id][1], STDOUT_FILENO);
	//printf("not this\n");
	pipes_close(pipes, proc->id + 1);
	if (is_redirect(proc))
		redirect_pipe(proc->io_info, info);
	if (ft_exec(proc->cmd, info) == -1)
		xperror("child");
	return (1/* status */);
}

void	pids_wait(pid_t pids[], int num_pids, int status)
{
	int	i;
	
	i = 0;
	while (i <= num_pids)
	{
		waitpid(pids[i], &status, 0);
		i++;
	}
}

//todo: num of fork return error
int	exec_multi_procs(t_proc *proc, t_info *info)
{
	int		pipes[MAX_PROC][2];
	pid_t	pids[MAX_PROC + 1];
	int		status;
	t_proc	*proc_p;

	proc_p = proc;
	while (proc_p)
	{
		xpipe(pipes[proc_p->id]);
		pids[proc_p->id] = xfork();
		if (pids[proc_p->id] == 0)
			status = child_proc(proc_p, pipes, info);
		if (!is_first_proc(proc_p))
		{
			close(pipes[proc_p->id - 1][0]);
			close(pipes[proc_p->id - 1][1]);
		}
		proc_p = proc_p->next;
	}
	pids_wait(pids, proc_num(proc), status);
	return (1);
}

int	exec_single_proc(t_proc *proc, t_info *info)
{
	int		status;
	pid_t	pid;
	pid_t	wpid;

	(void)info; // will use later
	pid = xfork();
	if (pid == 0)
	{
		if (is_redirect(proc))
			redirect_pipe(proc->io_info, info);
		if (ft_exec(proc->cmd, info) == -1)
			xperror("child");
	}
	wpid = waitpid(pid, &status, WUNTRACED);
	return (1); /* WEXITSTATUS(status) */
}

int	shell_launch(t_proc *proc, t_info *info)
{
	int	status;
	/* extern char **environ;
	
	info->environ = dict_to_array(info->env);
	environ = info->environ; */
	if (is_single_proc(proc))
		status = exec_single_proc(proc, info);
	else
		status = exec_multi_procs(proc, info);
	return (1/* status */);
}

int	exec_shell(t_proc *proc, t_info *info)
{
	if (proc == NULL)//when proc is NULL, check it later
		return (1);
	//make a wrap func	
	if (is_builtin(proc->cmd))
		return (exec_builtin(proc, info));
	return (shell_launch(proc, info));
}