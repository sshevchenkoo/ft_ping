#ifndef FT_PING_H
# define FT_PING_H

# include <stddef.h>
# include <stdint.h>
# include <signal.h>
# include <netinet/in.h>

# define PROG_NAME		"ft_ping"

/* Exit codes used by inetutils ping */
# define EXIT_OK		0
# define EXIT_FAIL		1
# define EXIT_USAGE		64

# define ICMP_HDR_LEN	8
# define DATA_LEN		56
# define PACKET_LEN		(ICMP_HDR_LEN + DATA_LEN)
# define RECV_BUF_LEN	4096
# define INTERVAL_SEC	1

typedef struct s_opts
{
	int		verbose;
	char	*host;
}	t_opts;

typedef struct s_ping
{
	t_opts				opts;
	int					fd;
	struct sockaddr_in	dest;
	char				*hostname;
	uint16_t			id;
	uint16_t			seq;
	size_t				sent;
	size_t				received;
	size_t				dups;
	double				rtt_min;
	double				rtt_max;
	double				rtt_sum;
	double				rtt_sumsq;
	unsigned char		seen[65536 / 8];
}	t_ping;

/* Set by the SIGINT handler, checked by the main loop */
extern volatile sig_atomic_t	g_stop;

/* args.c */
void		parse_args(int argc, char **argv, t_opts *opts);

/* usage.c */
void		print_help(void);
void		print_try_help(void);

/* init.c */
int			resolve_host(t_ping *ping);
int			open_socket(t_ping *ping);

/* packet.c */
uint16_t	checksum(const void *data, size_t len);
int			send_echo(t_ping *ping);

/* receive.c */
void		receive_packet(t_ping *ping);

/* loop.c */
int			ping_loop(t_ping *ping);

/* stats.c */
void		print_stats(const t_ping *ping);

#endif
