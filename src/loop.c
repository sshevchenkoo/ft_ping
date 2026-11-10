#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include "ft_ping.h"

/* Time left until the next send; zero if it is already due. */
static struct timeval	time_left(const struct timeval *last)
{
	struct timeval	now;
	struct timeval	left;

	gettimeofday(&now, NULL);
	left.tv_sec = last->tv_sec + INTERVAL_SEC - now.tv_sec;
	left.tv_usec = last->tv_usec - now.tv_usec;
	if (left.tv_usec < 0)
	{
		left.tv_sec--;
		left.tv_usec += 1000000;
	}
	if (left.tv_sec < 0)
	{
		left.tv_sec = 0;
		left.tv_usec = 0;
	}
	return (left);
}

/*
** Send one request, then wait on the socket with select() until the interval
** is over: a readable socket means a packet to read, a timeout means it is
** time to send again. SIGINT interrupts select() (EINTR) and sets g_stop.
*/
int	ping_loop(t_ping *ping)
{
	struct timeval	last;
	struct timeval	timeout;
	fd_set			fds;
	int				ready;

	if (send_echo(ping) < 0)
		return (-1);
	gettimeofday(&last, NULL);
	while (!g_stop)
	{
		FD_ZERO(&fds);
		FD_SET(ping->fd, &fds);
		timeout = time_left(&last);
		ready = select(ping->fd + 1, &fds, NULL, NULL, &timeout);
		if (ready < 0 && errno != EINTR)
		{
			fprintf(stderr, "%s: select: %s\n", PROG_NAME, strerror(errno));
			return (-1);
		}
		else if (ready > 0)
			receive_packet(ping);
		else if (ready == 0)
		{
			if (send_echo(ping) < 0)
				return (-1);
			gettimeofday(&last, NULL);
		}
	}
	return (0);
}
