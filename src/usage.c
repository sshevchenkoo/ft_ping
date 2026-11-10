#include <stdio.h>
#include "ft_ping.h"

void	print_help(void)
{
	printf("Usage: %s [OPTION...] HOST\n", PROG_NAME);
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
	printf("  -v, --verbose              verbose output\n");
	printf("  -?, --help                 give this help list\n");
}

void	print_try_help(void)
{
	fprintf(stderr,
		"Try '%s --help' or '%s --usage' for more information.\n",
		PROG_NAME, PROG_NAME);
}
