bin_PROGRAMS = rtime rtimed

rtime_SOURCES  = rtime.c rtime_proc_clnt.c
nodist_rtime_SOURCES = rtime.h rtime_clnt.c rtime_xdr.c

rtimed_LDADD = -lrt
rtimed_SOURCES  = rtimed.c rtime_proc_svc.c
nodist_rtimed_SOURCES = rtime.h rtime_svc.c rtime_xdr.c

BUILT_SOURCES = rtime.h rtime_clnt.c rtime_svc.c rtime_xdr.c
CLEANFILES = rtime.h rtime_clnt.c rtime_svc.c rtime_xdr.c

RPCGEN = rpcgen -C

rtime.h: rtime.x
	$(RPCGEN) -h $< >$@

rtime_clnt.c: rtime.x
	$(RPCGEN) -l $< >$@

rtime_svc.c: rtime.x
	$(RPCGEN) -m $< >$@

rtime_xdr.c: rtime.x
	$(RPCGEN) -c $< >$@

