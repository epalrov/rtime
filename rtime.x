/*
 * rtime.x: Remote time protocol
 */

typedef int rtime_clockid_t;

struct rtime_timespec {
	int tv_sec;            /* Seconds.  */
	int tv_nsec;           /* Nanoseconds.  */
};

struct rtime_clock_res {
	int clock;
};

struct rtime_clock_gettime_arg {
	rtime_clockid_t clock_id;
};
struct rtime_clock_gettime_res {
	int err;
	struct rtime_timespec *tp;
};

struct rtime_clock_settime_arg {
	rtime_clockid_t clock_id;
	struct rtime_timespec *tp;
};
struct rtime_clock_settime_res {
	int err;
};

program RTIME_PROG {
	version RTIME_VERS {
		void RTIME_NULL(void) = 0;
		rtime_clock_res RTIME_CLOCK(void) = 1;
		rtime_clock_gettime_res RTIME_CLOCK_GETTIME(rtime_clock_gettime_arg) = 2;
		rtime_clock_settime_res RTIME_CLOCK_SETTIME(rtime_clock_settime_arg) = 3;
	} = 1;
} = 98;

