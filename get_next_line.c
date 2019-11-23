/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_next_line.c                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: dpattij <dpattij@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2019/11/01 20:34:49 by dpattij        #+#    #+#                */
/*   Updated: 2019/11/23 19:16:44 by dpattij       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static int			get_fd_buffer(t_vecstr *buffers, int fd, t_pairedstr **out)
{
	t_size			idx;
	t_pairedstr		*cur;

	idx = 0;
	cur = idx < buffers->length ? buffers->raw.v[0] : NULL;
	while (idx < buffers->length && cur->key != fd)
	{
		cur = buffers->raw.v[idx];
		idx += 1;
	}
	if (buffers->length == 0 || (idx == buffers->length && cur->key != fd))
	{
		cur = malloc(sizeof(t_pairedstr));
		if (cur == NULL)
			return (STATUS_ERROR);
		cur->string = malloc(sizeof(char) * BUFFER_SIZE);
		cur->key = fd;
		cur->buf_used = 0;
		cur->head = 0;
		idx = -1;
	}
	if (idx == -1ULL && (cur->string == NULL || vecstr_push(buffers, cur) != 0))
		return (-(vecstr_drop(cur->string, 1, 1) && vecstr_drop(cur, 1, 1)));
	*out = cur;
	return (idx == -1ULL ? STATUS_BUFFER_IS_NEW : STATUS_BUFFER_IS_OLD);
}

static int			refit_buffer(t_vecstr *buffers, t_pairedstr *collected)
{
	t_size			idx;

	idx = 0;
	while (buffers->raw.v[idx] != collected)
		idx += 1;
	while (idx < (buffers->length) - 1)
	{
		buffers->raw.v[idx] = buffers->raw.v[idx + 1];
		idx -= 1;
	}
	free(collected->string);
	free(collected);
	buffers->length -= 1;
	return (vecstr_resize(buffers, buffers->length));
}

static int			append_to_buffer
(
	t_pairedstr *cur_buf,
	t_vecstr *cur_line,
	t_size n
)
{
	t_size			idx;

	idx = cur_buf->head;
	while (idx < BUFFER_SIZE && idx < (cur_buf->head + n))
	{
		if (cur_buf->string[idx] == '\n')
		{
			cur_buf->head = idx + 1;
			return (STATUS_LINE_READ);
		}
		if (vecstr_push(cur_line, (void *)(t_size)(cur_buf->string[idx])) != 0)
			return (STATUS_ERROR);
		idx += 1;
	}
	cur_buf->head = 0;
	return (STATUS_SUCCESS);
}

static int			read_until_newline
(
	int fd,
	t_pairedstr *cur_buf,
	t_vecstr *cur_line,
	char **line
)
{
	t_ssize			result;

	result = fd == -1 ? -10 : read(fd, cur_buf->string, BUFFER_SIZE);
	while (result > 0)
	{
		cur_buf->buf_used = result;
		result = append_to_buffer(cur_buf, cur_line, result);
		if (result != STATUS_SUCCESS)
			break ;
		result = read(fd, cur_buf->string, BUFFER_SIZE);
	}
	if (result >= 0 || fd == -1)
	{
		if (vecstr_push(cur_line, NULL) != STATUS_SUCCESS
			|| vecstr_resize(cur_line, cur_line->length) != STATUS_SUCCESS)
			return (STATUS_ERROR);
		*line = cur_line->raw.c;
		free(cur_line);
	}
	return (result);
}

int					get_next_line(int fd, char **line)
{
	static t_vecstr	*buffers;
	t_vecstr		*c_line;
	t_pairedstr		*c_buf;
	t_ssize			result;

	if (line == NULL || fd < 0 || vecstr_maybe_new(&buffers, TaggedVector) != 0)
		return (STATUS_ERROR);
	result = get_fd_buffer(buffers, fd, &c_buf);
	if ((vecstr_new(&c_line, TaggedString) == -1) || result == STATUS_ERROR)
		return (vecstr_drop(c_line, STATUS_ERROR, 0));
	if (result == STATUS_BUFFER_IS_OLD)
	{
		result = append_to_buffer(c_buf, c_line, c_buf->buf_used - c_buf->head);
		if (result == 1)
			result = read_until_newline(-1, c_buf, c_line, line);
		if (result <= STATUS_ERROR)
			return (result != -1 ? 1
			: vecstr_drop(c_buf, -1, 1) + vecstr_drop(c_line, 0, 0));
	}
	result = read_until_newline(fd, c_buf, c_line, line);
	if (result == STATUS_EOF_REACHED || result == STATUS_ERROR)
		refit_buffer(buffers, c_buf);
	if (result == STATUS_ERROR)
		vecstr_drop(c_line, 0, 0);
	return (result);
}
