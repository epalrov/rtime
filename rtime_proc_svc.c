/*
 * rtime_proc_svc.c - Remote TIME server procedures
 *
 * Copyright (C) 2014 Paolo Rovelli
 *
 * Author: Paolo Rovelli <paolorovelli@yahoo.it>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <rpc/rpc.h>

#include <time.h>
#include "rtime.h"

void *rtime_null_1_svc(void *arg, struct svc_req *UNUSED)
{
	static int res; /* must be static! */

	return (&res);
}

rtime_clock_res *rtime_clock_1_svc(void *arg, struct svc_req *UNUSED)
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

