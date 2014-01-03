/*
 * rtime.c - Remote TIME service, client application
 *
 * Copyright (C) 2014 Paolo Rovelli
 *
 * Author: Paolo Rovelli <paolorovelli@yahoo.it>
 */

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <rpc/rpc.h>

#include "rtime.h"

#define RTIME_HOSTNAME "localhost"
#define RTIME_SERVICE_TYPE "tcp"

static void rtime_usage(FILE * out)
{
	static const char usage_str[] =
		("Usage:                                                   \n"
		"  rtime [options] procedure [args]                      \n\n"
		"Options:                                                  \n"
		"  -a | --address    rtime server addr <hostname/pathname> \n"
		"  -t | --type       rtime service type <tcp/udp/unix>     \n"
		"  -v | --version    show the program version and exit     \n"
		"  -h | --help       show this help and exit             \n\n"
		"Examples:                                                 \n"
		"  rtime clock_settime 10.5                                \n"
		"  rtime -a localhost -t tcp clock_gettime                 \n"
		"  rtime -a /var/run/rtime.socket -t unix clock_gettime  \n\n");

	fprintf(out, "%s", usage_str);
	fflush(out);
	return;
}

static void rtime_version(FILE * out)
{
	static const char prog_str[] = "rtime";
	static const char ver_str[] = "1.0";
	static const char author_str[] = "Paolo Rovelli";

	fprintf(out, "%s %s written by %s\n", prog_str, ver_str, author_str);
	fflush(out);
	return;
}

static const struct option rtime_options[] = {
	{ "addr", required_argument, NULL, 'a' },
	{ "type", required_argument, NULL, 't' },
	{ "version", no_argument, NULL, 'v' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 },
};

struct procedure {
	char *proc_name;
	int proc_no;
};

static const struct procedure rtime_procedures[] = {
	{ "null", RTIME_NULL },
	{ "clock", RTIME_CLOCK },
	{ "clock_gettime", RTIME_CLOCK_GETTIME },
	{ "clock_settime", RTIME_CLOCK_SETTIME },
};

int main(int argc, char *argv[])
{
	int i, opt;
	char *addr = RTIME_HOSTNAME;
	char *type = RTIME_SERVICE_TYPE;

	int proc_no, proc_max;
	char *proc_name;

	CLIENT *cl;

	int ret;
	clock_t clock;
	clockid_t clock_id;
        struct timespec tp;

	/* parse RTIME client command line options */
	while ((opt = getopt_long(argc, argv, "a:t:vh", rtime_options,
				NULL)) != -1) {
		switch (opt) {
		case 'a':
			addr = optarg;
			break;
		case 't':
			type = optarg;
			break;
		case 'v':
			rtime_version(stdout);
			exit(EXIT_SUCCESS);
		case 'h':
			rtime_usage(stdout);
			exit(EXIT_SUCCESS);
		default:
			rtime_usage(stderr);
			exit(EXIT_FAILURE);
		}
	}

	/* move index to the remote procedure name */
	argc -= optind;
	argv += optind;

	if (argc > 0) {
		proc_name = argv[0];
	} else {
		proc_name = rtime_procedures[0].proc_name;
	}
	proc_max = sizeof(rtime_procedures)/sizeof(rtime_procedures[0]);
	for (i = 0; i < proc_max; i++) {
		if (strcmp(proc_name, rtime_procedures[i].proc_name) == 0)
			break;
	}
	if (i < proc_max) {
		proc_no = rtime_procedures[i].proc_no;
	} else {
		fprintf(stderr, "invalid remote procedure: '%s'\n", proc_name);
		exit(EXIT_FAILURE);
	}

	/* move index to the remote procedure arguments */
	argc -= 1;
	argv += 1;


	cl = clnt_create(addr, RTIME_PROG, RTIME_VERS, type);
	if (!cl) {
		clnt_pcreateerror(addr);
		exit(EXIT_FAILURE);
	}

	switch (proc_no) {
	case RTIME_NULL:
		ret = rtime_null_1_clnt(cl);
		break;
	case RTIME_CLOCK:
		ret = rtime_clock_1_clnt(cl, &clock);
		if (!ret)
			printf("remote clock(): %d\n", (int)clock);
		break;
	case RTIME_CLOCK_GETTIME:
		/* FIXME retrieve arguments from argv! */
		clock_id = 0;
		ret = rtime_clock_gettime_1_clnt(cl, clock_id, &tp);
		if (!ret)
			printf("remote clock_gettime(): %d.%09d\n",
				(int)tp.tv_sec, (int)tp.tv_nsec);
		else
			printf("remote clock_gettime() error: %d\n", ret);
		break;
	case RTIME_CLOCK_SETTIME:
		/* FIXME retrieve arguments from argv! */
		clock_id = 0;
		tp.tv_sec = 10;
		tp.tv_nsec = 10;
		ret = rtime_clock_settime_1_clnt(cl, clock_id, &tp);
		if (ret)
			printf("remote clock_settime() error: %d\n", ret);
		break;
	}	
	
	clnt_destroy(cl);

	exit(ret ? EXIT_FAILURE : EXIT_SUCCESS);
}

