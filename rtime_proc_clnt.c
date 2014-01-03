/*
 * rtime_proc_clnt.c - Remote TIME client procedures
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

int rtime_null_1_clnt(CLIENT *cl)
{
	int ret = 0;
	void *arg = NULL;
	void *res;

	res = rtime_null_1(arg, cl);
	if (!res) {
		clnt_perror(cl, __func__);
		ret = -1;
	}

	return ret;
}

int rtime_clock_1_clnt(CLIENT *cl, clock_t *clock)
{
	int ret = 0;
	void *arg = NULL;
	rtime_clock_res *res;

	res = rtime_clock_1(arg, cl);
	if (!res) {
		clnt_perror(cl, __func__);
		ret = -1;
	}
	*clock = res->clock;

	return ret;
}

int rtime_clock_gettime_1_clnt(CLIENT *cl, clockid_t clock_id,
	struct timespec *tp)
{
	int ret = 0;
	rtime_clock_gettime_arg *arg;
	rtime_clock_gettime_res *res;
		
	arg = calloc(1, sizeof(*arg));
	arg->clock_id = (rtime_clockid_t)clock_id;
	res = rtime_clock_gettime_1(arg, cl);
	if (!res) {
		clnt_perror(cl, __func__);
		ret = -1;
	} else {
		tp->tv_sec = res->tp->tv_sec;
		tp->tv_nsec = res->tp->tv_nsec;
		ret = res->err;
	}
	free(arg);

	return ret;
}

int rtime_clock_settime_1_clnt(CLIENT *cl, clockid_t clock_id,
	struct timespec *tp)
{
	int ret = 0;
	rtime_clock_settime_arg *arg;
	rtime_clock_settime_res *res;
		
	arg = calloc(1, sizeof(*arg));
	arg->clock_id = (rtime_clockid_t)clock_id;
	arg->tp = calloc(1, sizeof(*(arg->tp)));
	arg->tp->tv_sec = tp->tv_sec;
	arg->tp->tv_nsec = tp->tv_nsec;
	res = rtime_clock_settime_1(arg, cl);
	if (!res) {
		clnt_perror(cl, __func__);
		ret = -1;
	} else {
		ret = res->err;
	}
	free(arg->tp);
	free(arg);
	
	return ret;
}

