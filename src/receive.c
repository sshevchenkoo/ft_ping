#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include "ft_ping.h"

/* Round-trip time in ms: now minus the timestamp we put in the request. */
static double	compute_rtt(const unsigned char *data)
{
	struct timeval	sent;
	struct timeval	now;

	memcpy(&sent, data, sizeof(sent));
	gettimeofday(&now, NULL);
	return ((now.tv_sec - sent.tv_sec) * 1000.0
		+ (now.tv_usec - sent.tv_usec) / 1000.0);
}

static void	update_rtt(t_ping *ping, double rtt)
{
	if (ping->received + ping->dups == 0 || rtt < ping->rtt_min)
		ping->rtt_min = rtt;
	if (ping->received + ping->dups == 0 || rtt > ping->rtt_max)
		ping->rtt_max = rtt;
	ping->rtt_sum += rtt;
	ping->rtt_sumsq += rtt * rtt;
}

static void	handle_echo_reply(t_ping *ping, const struct ip *ip,
		const struct icmp *icmp, size_t icmp_len)
{
	uint16_t	seq;
	double		rtt;
	int			dup;

	if (ntohs(icmp->icmp_id) != ping->id)
		return ;
	seq = ntohs(icmp->icmp_seq);
	rtt = compute_rtt((const unsigned char *)icmp + ICMP_HDR_LEN);
	update_rtt(ping, rtt);
	dup = ping->seen[seq / 8] & (1 << (seq % 8));
	ping->seen[seq / 8] |= (1 << (seq % 8));
	if (dup)
		ping->dups++;
	else
		ping->received++;
	printf("%zu bytes from %s: icmp_seq=%u ttl=%d time=%.3f ms",
		icmp_len, inet_ntoa(ip->ip_src), seq, ip->ip_ttl, rtt);
	if (dup)
		printf(" (DUP!)");
	printf("\n");
}

/*
** A raw socket hands us the whole IP packet: IP header first (its length is
** ip_hl * 4), then the ICMP message. It also delivers every ICMP packet the
** host receives, so anything that is not ours must be filtered out.
*/
void	receive_packet(t_ping *ping)
{
	unsigned char		buf[RECV_BUF_LEN];
	struct sockaddr_in	from;
	socklen_t			from_len;
	ssize_t				n;
	size_t				hdr_len;

	from_len = sizeof(from);
	n = recvfrom(ping->fd, buf, sizeof(buf), 0,
			(struct sockaddr *)&from, &from_len);
	if (n < 0)
	{
		if (errno != EINTR)
			fprintf(stderr, "%s: recvfrom: %s\n", PROG_NAME, strerror(errno));
		return ;
	}
	if ((size_t)n < sizeof(struct ip))
		return ;
	hdr_len = ((struct ip *)buf)->ip_hl * 4;
	if ((size_t)n < hdr_len + PACKET_LEN)
		return ;
	if (checksum(buf + hdr_len, n - hdr_len) != 0)
	{
		fprintf(stderr, "checksum mismatch from %s\n",
			inet_ntoa(from.sin_addr));
		return ;
	}
	if (((struct icmp *)(buf + hdr_len))->icmp_type == ICMP_ECHOREPLY)
		handle_echo_reply(ping, (struct ip *)buf,
			(struct icmp *)(buf + hdr_len), n - hdr_len);
	/* TODO stage 7: ICMP errors (time exceeded, unreachable) and -v dump */
}
