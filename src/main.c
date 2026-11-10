#include "ft_ping.h"

int	main(int argc, char **argv)
{
	t_opts	opts;

	parse_args(argc, argv, &opts);
	/* TODO stage 2: resolve opts.host, open the raw ICMP socket */
	return (EXIT_OK);
}
