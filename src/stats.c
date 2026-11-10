#include <math.h>
#include <stdio.h>
#include "ft_ping.h"

static void	print_rtt(const t_ping *ping)
{
	double	total;
	double	avg;
	double	variance;

	total = ping->received + ping->dups;
	avg = ping->rtt_sum / total;
	variance = ping->rtt_sumsq / total - avg * avg;
	if (variance < 0)
		variance = 0;
	printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
		ping->rtt_min, avg, ping->rtt_max, sqrt(variance));
}

void	print_stats(const t_ping *ping)
{
	printf("--- %s ping statistics ---\n", ping->hostname);
	printf("%zu packets transmitted, ", ping->sent);
	printf("%zu packets received, ", ping->received);
	if (ping->dups > 0)
		printf("+%zu duplicates, ", ping->dups);
	if (ping->sent > 0)
	{
		if (ping->received > ping->sent)
			printf("-- somebody is printing forged packets!");
		else
			printf("%d%% packet loss",
				(int)(((ping->sent - ping->received) * 100) / ping->sent));
	}
	printf("\n");
	if (ping->received + ping->dups > 0)
		print_rtt(ping);
}
