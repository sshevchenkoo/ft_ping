#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ft_ping.h"

static void	parse_long(char *arg, t_opts *opts)
{
	if (strcmp(arg, "--verbose") == 0)
		opts->verbose = 1;
	else if (strcmp(arg, "--help") == 0)
	{
		print_help();
		exit(EXIT_OK);
	}
	else
	{
		fprintf(stderr, "%s: unrecognized option '%s'\n", PROG_NAME, arg);
		print_try_help();
		exit(EXIT_USAGE);
	}
}

/* Handles grouped short options, e.g. "-v" or "-vv" */
static void	parse_short(char *arg, t_opts *opts)
{
	int	i;

	i = 1;
	while (arg[i])
	{
		if (arg[i] == 'v')
			opts->verbose = 1;
		else if (arg[i] == '?')
		{
			print_help();
			exit(EXIT_OK);
		}
		else
		{
			fprintf(stderr, "%s: invalid option -- '%c'\n",
				PROG_NAME, arg[i]);
			print_try_help();
			exit(EXIT_USAGE);
		}
		i++;
	}
}

void	parse_args(int argc, char **argv, t_opts *opts)
{
	int	i;
	int	only_hosts;

	memset(opts, 0, sizeof(*opts));
	only_hosts = 0;
	i = 1;
	while (i < argc)
	{
		if (!only_hosts && strcmp(argv[i], "--") == 0)
			only_hosts = 1;
		else if (!only_hosts && strncmp(argv[i], "--", 2) == 0)
			parse_long(argv[i], opts);
		else if (!only_hosts && argv[i][0] == '-' && argv[i][1] != '\0')
			parse_short(argv[i], opts);
		else if (opts->host == NULL)
			opts->host = argv[i];
		i++;
	}
	if (opts->host == NULL)
	{
		fprintf(stderr, "%s: missing host operand\n", PROG_NAME);
		print_try_help();
		exit(EXIT_USAGE);
	}
}
