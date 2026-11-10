#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "ft_ping.h"

volatile sig_atomic_t	g_stop = 0;

static void	on_sigint(int sig)
{
	(void)sig;
	g_stop = 1;
}

/* No SA_RESTART: we want SIGINT to interrupt select() in the main loop. */
static void	setup_signals(void)
{
	struct sigaction	sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = on_sigint;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
}

static void	print_header(const t_ping *ping)
{
	printf("PING %s (%s): %d data bytes", ping->hostname,
		inet_ntoa(ping->dest.sin_addr), DATA_LEN);
	if (ping->opts.verbose)
		printf(", id 0x%04x = %u", ping->id, ping->id);
	printf("\n");
}

static int	cleanup(t_ping *ping, int status)
{
	if (ping->fd >= 0)
		close(ping->fd);
	free(ping->hostname);
	return (status);
}

int	main(int argc, char **argv)
{
	static t_ping	ping;
	int				failed;

	ping.fd = -1;
	parse_args(argc, argv, &ping.opts);
	if (open_socket(&ping) < 0 || resolve_host(&ping) < 0)
		return (cleanup(&ping, EXIT_FAIL));
	setup_signals();
	print_header(&ping);
	failed = ping_loop(&ping) < 0;
	if (!failed)
		print_stats(&ping);
	return (cleanup(&ping, failed || ping.received == 0));
}
