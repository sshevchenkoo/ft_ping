#ifndef FT_PING_H
# define FT_PING_H

# define PROG_NAME	"ft_ping"

/* Exit codes used by inetutils ping */
# define EXIT_OK	0
# define EXIT_FAIL	1
# define EXIT_USAGE	64

typedef struct s_opts
{
	int		verbose;
	char	*host;
}	t_opts;

/* args.c */
void	parse_args(int argc, char **argv, t_opts *opts);

/* usage.c */
void	print_help(void);
void	print_try_help(void);

#endif
