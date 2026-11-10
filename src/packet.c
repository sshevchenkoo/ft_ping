#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include "ft_ping.h"

/*
** Internet checksum (RFC 1071): sum the data as 16-bit words, fold the
** carries back into the low 16 bits, then invert.
** Running it over a packet that already contains a valid checksum gives 0.
*/
uint16_t	checksum(const void *data, size_t len)
{
	const unsigned char	*bytes;
	uint32_t			sum;
	uint16_t			word;

	bytes = data;
	sum = 0;
	while (len > 1)
	{
		memcpy(&word, bytes, 2);
		sum += word;
		bytes += 2;
		len -= 2;
	}
	if (len == 1)
	{
		word = 0;
		memcpy(&word, bytes, 1);
		sum += word;
	}
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);
	return ((uint16_t)~sum);
}

/*
** Builds one ICMP Echo Request: 8-byte header + 56 bytes of data.
** The send time goes at the start of the data so the reply carries it back.
*/
static void	build_packet(t_ping *ping, unsigned char *packet)
{
	struct icmp		*icmp;
	struct timeval	now;
	size_t			i;

	i = 0;
	while (i < DATA_LEN)
	{
		packet[ICMP_HDR_LEN + i] = (unsigned char)i;
		i++;
	}
	gettimeofday(&now, NULL);
	memcpy(packet + ICMP_HDR_LEN, &now, sizeof(now));
	icmp = (struct icmp *)packet;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_cksum = 0;
	icmp->icmp_id = htons(ping->id);
	icmp->icmp_seq = htons(ping->seq);
	icmp->icmp_cksum = checksum(packet, PACKET_LEN);
}

int	send_echo(t_ping *ping)
{
	unsigned char	packet[PACKET_LEN];
	ssize_t			ret;

	build_packet(ping, packet);
	ping->seen[ping->seq / 8] &= ~(1 << (ping->seq % 8));
	ret = sendto(ping->fd, packet, PACKET_LEN, 0,
			(struct sockaddr *)&ping->dest, sizeof(ping->dest));
	if (ret < 0)
	{
		fflush(stdout);
		fprintf(stderr, "%s: sending packet: %s\n", PROG_NAME,
			strerror(errno));
		return (-1);
	}
	ping->seq++;
	ping->sent++;
	return (0);
}
