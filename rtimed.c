/*
 * rtimed.c - Remote TIME service, server application
 *
 * Copyright (C) 2014 Paolo Rovelli
 *
 * Author: Paolo Rovelli <paolorovelli@yahoo.it>
 */

#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <rpc/rpc.h>

#include "rtime.h"

#define RTIME_HOSTNAME "localhost"
#define RTIME_SERVICE_TYPE "tcp"

extern void rtime_prog_1(struct svc_req *rqstp, register SVCXPRT *sv);

void *rtime_null_1_svc(void *arg,
	struct svc_req *UNUSED)
{
	static int res; /* must be static! */

	return (&res);
}

rtime_clock_res *rtime_clock_1_svc(void *arg,
	struct svc_req *UNUSED)
{
	static rtime_clock_res res; /* must be static! */

	res.clock = clock();
	return (&res);
}

rtime_clock_gettime_res *rtime_clock_gettime_1_svc(rtime_clock_gettime_arg *arg,
	struct svc_req *UNUSED)
{
	static rtime_clock_gettime_res res; /* must be static! */
	static rtime_timespec tp; /* must be static! */

	res.tp = &tp;
	res.err = clock_gettime((clockid_t)(arg->clock_id),
		(struct timespec *)(res.tp));
	return (&res);
}

rtime_clock_settime_res *rtime_clock_settime_1_svc(rtime_clock_settime_arg *arg,
	struct svc_req *UNUSED)
{
	static rtime_clock_settime_res res; /* must be static! */
	
	res.err = clock_settime((clockid_t)arg->clock_id,
		(struct timespec *)(arg->tp));
	return (&res);
}

static void rtimed_usage(FILE * out)
{
	static const char usage_str[] =
		("Usage:                                                   \n"
		"  rtimed [options]                                      \n\n"
		"Options:                                                  \n"
		"  -a | --address    rtime server addr <hostname/pathname> \n"
		"  -t | --type       rtime service type <tcp/udp/unix>     \n"
		"  -v | --version    show the program version and exit     \n"
		"  -h | --help       show this help and exit             \n\n"
		"Examples:                                                 \n"
		"  rtimed                                                  \n"
		"  rtimed -a localhost -t udp                              \n"
		"  rtimed -a /var/run/rtime.socket -t unix               \n\n");

	fprintf(out, "%s", usage_str);
	fflush(out);
	return;
}

static void rtimed_version(FILE * out)
{
	static const char prog_str[] = "rtimed";
	static const char ver_str[] = "1.0";
	static const char author_str[] = "Paolo Rovelli";

	fprintf(out, "%s %s written by %s\n", prog_str, ver_str, author_str);
	fflush(out);
	return;
}

static const struct option rtimed_options[] = {
	{ "addr", required_argument, NULL, 'a' },
	{ "type", required_argument, NULL, 't' },
	{ "version", no_argument, NULL, 'v' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 },
};

static char *rtime_unix_sock;

static void rtime_sigterm_handler(int signal)
{
	if (rtime_unix_sock)
		unlink(rtime_unix_sock);
	else
		pmap_unset(RTIME_PROG, RTIME_VERS);
        
        exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	int i, opt;
	char *addr = RTIME_HOSTNAME;
	char *type = RTIME_SERVICE_TYPE;

	int proto;
	SVCXPRT *sv;

	/* parse RTIME client command line options */
	while ((opt = getopt_long(argc, argv, "a:t:vh", rtimed_options,
				NULL)) != -1) {
		switch (opt) {
		case 'a':
			addr = optarg;
			break;
		case 't':
			type = optarg;
			break;
		case 'v':
			rtimed_version(stdout);
			exit(EXIT_SUCCESS);
		case 'h':
			rtimed_usage(stdout);
			exit(EXIT_SUCCESS);
		default:
			rtimed_usage(stderr);
			exit(EXIT_FAILURE);
		}
	}

	/* register RTIME signal handlers */
	signal(SIGINT, rtime_sigterm_handler);
	signal(SIGTERM, rtime_sigterm_handler);

	/* create RTIME service */
	if (strcmp(type, "tcp") == 0) {
		pmap_unset(RTIME_PROG, RTIME_VERS);
		sv = svctcp_create(RPC_ANYSOCK, 0, 0);
		proto = IPPROTO_TCP;
	} else if (strcmp(type, "udp") == 0) {
		pmap_unset(RTIME_PROG, RTIME_VERS);
		sv = svcudp_create(RPC_ANYSOCK);
		proto = IPPROTO_UDP;
	} else if (strcmp(type, "unix") == 0) {
		rtime_unix_sock = addr;
		sv = svcunix_create(RPC_ANYSOCK, 0, 0, rtime_unix_sock);
		proto = 0;
	} else {
		fprintf(stderr, "invalid service type '%s'\n", type);
		exit(EXIT_FAILURE);
	}

	if (!sv) {
		fprintf(stderr, "unable to create service type '%s'\n", type);
		exit(EXIT_FAILURE);
	}

	/* register RTIME service */
	if (!svc_register(sv, RTIME_PROG, RTIME_VERS, rtime_prog_1, proto)) {
		fprintf(stderr, "unable to register RTIME program\n");
		exit(EXIT_FAILURE);
	}

	/* start RTIME service */
	svc_run();

	exit(EXIT_FAILURE);
}

