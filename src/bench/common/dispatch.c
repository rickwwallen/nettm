#include <pktbuff.h>

#ifndef DISPATCH_NUM
#error need to define how many packet recipients
#endif

#if DISPATCH_NUM > 0

#define DISPATCH_Q 10

struct ioq_header* dispatched_msg[MAX_THREAD][DISPATCH_Q];
int num_dispatched[MAX_THREAD] = {0,0,0,0,0,0,0,0
				  /*,0,0,0,0,0,0,0,0*/};


struct ioq_header* do_dispatch_core(int flowid, int myid, struct ioq_header* ioq)
{
  int dest = (flowid % DISPATCH_NUM);

  if((dest < 0) || (dest > DISPATCH_NUM))
    {
      log("DISPATCH error in dest, val %d\n", dest);
      while(1);
    }

  if(dest != myid)
    {
      int success = 0;

      while(!success)
	{
	  nf_lock(DISPAT_LOCK);
	  if(num_dispatched[dest] < DISPATCH_Q)
	    {
	      dispatched_msg[dest][num_dispatched[dest]] = ioq;
	      num_dispatched[dest]++;
	      success = 1;
	      log("DISPATCH added packet to queue %d\n", dest);
	    }
	  nf_unlock(DISPAT_LOCK);
	  if(!success)
	    {
	      relax();
	      log("DISPATCH waited before retrying to add packet to queue %d\n", dest);
	    }
	}
      return NULL;
    }
  else
    return ioq;
}

struct ioq_header* do_dispatch(int myid, struct ioq_header* ioq)
{
  // assume no options
#define IPOFF (sizeof(struct ioq_header)+14)

  int bytes = ntohs(ioq->byte_length);
  if(bytes < (IPOFF+sizeof(struct iphdr)))
    return ioq; // do not dispatch, too short
  
  struct iphdr* hh = (struct iphdr*)(&(((char*)ioq)[IPOFF]));
  unsigned sum = hh->saddr_h + hh->saddr_l + hh->daddr_h + hh->daddr_l; 

  return do_dispatch_core(sum, myid, ioq);
}

int get_dispatch_id(int myid)
{
#if (DISPATCH_NUM == MAX_THREAD)
  return myid;
#elif (DISPATCH_NUM == NUM_CPU)
  return (myid >= (MAX_THREAD/2));
#else
#error
  while(1);
  return 0;
#endif
}

struct ioq_header* get_dispatch_pkt(int myid)
{
  int qid = get_dispatch_id(myid);

  struct ioq_header* ret = NULL;
  // first, check in private waiting list
  nf_lock(DISPAT_LOCK);
  if(num_dispatched[qid] > 0)
    {
      ret = dispatched_msg[qid][num_dispatched[qid]-1];
      num_dispatched[qid] -= 1;
      log("DISPATCH got packet from queue %d\n", qid);
    }
  nf_unlock(DISPAT_LOCK);
  return ret;
}


int get_dispatch_global_lock_id(int lock_id, int myid)
{
#if DYN_LOCK_BASE == LOCK_DS1
// assume there is only one global lock id

#if DISPATCH_NUM == MAX_THREAD
  return -1;
#elif DISPATCH_NUM == NUM_CPU
  return lock_id + get_dispatch_id(myid);
#else
#error
  return 0;
#endif


#else
  // code is not planned for this, the flow_id would need to be pushed by
  // num_global*DISPATCH_NUM
  while(1);
  return 0;
#endif
}

int get_dispatch_flow_lock_id(int lock_id, int myid)
{
#if DISPATCH_NUM == MAX_THREAD
  return -1;
#elif DISPATCH_NUM == NUM_CPU

  int mid = get_dispatch_id(myid);
  int newspace = DYN_LOCK_SPACE/DISPATCH_NUM;
  int newbase = DYN_LOCK_BASE + DISPATCH_NUM + mid*newspace;
  int lid = (lock_id%newspace)+newbase;
  log("DISPATCH thread %d assigned lock id %d (space %d base %d)\n", myid, lid, newspace, newbase);
  return lid;
#else
#error
  return 0;
#endif
}

void dispatch_get_lock(int lock_id)
{
  if(lock_id == -1)
    return;
  else
    nf_lock(lock_id);
}

void dispatch_put_lock(int lock_id)
{
  if(lock_id == -1)
    return;
  else
    nf_unlock(lock_id);
}

#endif
