#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "ft_ping.h"

/*
** Turns opts.host (IPv4 address or hostname) into a sockaddr_in.
** The name printed in the header is the canonical one, as the reference does.
*/
int	resolve_host(t_ping *ping)
{
	struct addrinfo	hints;
	struct addrinfo	*res;
	const char		*name;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_CANONNAME;
	if (getaddrinfo(ping->opts.host, NULL, &hints, &res) != 0)
	{
		fprintf(stderr, "%s: unknown host\n", PROG_NAME);
		return (-1);
	}
	memcpy(&ping->dest, res->ai_addr, sizeof(ping->dest));
	name = ping->opts.host;
	if (res->ai_canonname != NULL)
		name = res->ai_canonname;
	ping->hostname = strdup(name);
	freeaddrinfo(res);
	if (ping->hostname == NULL)
	{
		fprintf(stderr, "%s: %s\n", PROG_NAME, strerror(errno));
		return (-1);
	}
	return (0);
}

/* A raw ICMP socket needs root (or CAP_NET_RAW). */
int	open_socket(t_ping *ping)
{
	ping->fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (ping->fd < 0)
	{
		if (errno == EPERM || errno == EACCES)
			fprintf(stderr, "%s: Lacking privilege for icmp socket.\n",
				PROG_NAME);
		else
			fprintf(stderr, "%s: %s\n", PROG_NAME, strerror(errno));
		return (-1);
	}
	ping->id = getpid() & 0xFFFF;
	return (0);
}
