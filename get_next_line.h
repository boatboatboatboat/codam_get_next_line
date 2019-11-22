/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_next_line.h                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: dpattij <dpattij@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2019/11/01 20:15:45 by dpattij        #+#    #+#                */
/*   Updated: 2019/11/05 23:15:19 by dpattij       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdlib.h>
# include <unistd.h>

# ifndef NULL
#  define NULL (void *)0
# endif

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 32
# endif

# define VECSTR_DEFAULT_CAPACITY 4
# define STATUS_ERROR -1
# define STATUS_EOF_REACHED 0
# define STATUS_LINE_READ 1
# define STATUS_SUCCESS 0
# define STATUS_BUFFER_IS_OLD 1
# define STATUS_BUFFER_IS_NEW 0

# define CAST(value, into) ((into)value)

typedef unsigned long long t_size;
typedef long long t_ssize;

union					u_varraw
{
	void				**v;
	char				*c;
};

enum					e_vecstr_tag
{
	TaggedString,
	TaggedVector
};

typedef struct			s_vecstr
{
	union u_varraw		raw;
	t_size				length;
	t_size				capacity;
	enum e_vecstr_tag	tag;
}						t_vecstr;

typedef struct			s_pairedstr
{
	int					key;
	char				*string;
	t_size				head;
	t_size				buf_used;
}						t_pairedstr;

int						vecstr_new(t_vecstr **target, enum e_vecstr_tag tag);
int						vecstr_resize(t_vecstr *self, t_size new_size);
int						vecstr_push(t_vecstr *self, void *value);
int						get_next_line(int fd, char **line);
int						vecstr_drop(void *any, int passthrough, int just_free);
int						vecstr_maybe_new(
	t_vecstr **target, enum e_vecstr_tag tag);

#endif
