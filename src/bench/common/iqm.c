#include "support.h"
#include <stdarg.h>

#ifdef DEBUG
#include <malloc.h>
#include <stdio.h>
#endif

#include "pktbuff.h"
#define PKTIN_OFFLOAD_ONCHIP 10
#define PKTIN_OFFLOAD_MAX       3
#define PKTIN_OFFLOAD_THRESHOLD  (PKTIN_OFFLOAD_ONCHIP-MAX_THREAD+2)
#define PKTIN_OFFLOAD_BUF       1600

t_addr static_offload[PKTIN_OFFLOAD_MAX][PKTIN_OFFLOAD_BUF]; // array of the offloaded packets
int static_offload_filled[PKTIN_OFFLOAD_MAX]; // are the slots taken or not in the array
int static_offload_taken[PKTIN_OFFLOAD_MAX];
int num_static_offload=0;                // number of slots taken
int packets_not_in_flight = PKTIN_OFFLOAD_ONCHIP;  // number of free'ed input slots, will go up to 10 at initilization
int old_packets_not_in_flight = PKTIN_OFFLOAD_ONCHIP;  // number of free'ed input slots, will go up to 10 at initilization

void nf_iqm_init()
{
  int i;
  for(i=0; i<PKTIN_OFFLOAD_MAX; i++)
    {
      static_offload_filled[i]  = 0;
      static_offload_taken[i] = 0;
    }
}

t_addr* nf_iqm_pktin_pop()
{
  uint val = NOT_A_PACKET;
  int i;

  while(1)
    {
      val = (uint)nf_pktin_pop();
      
      if(nf_pktin_is_valid((t_addr*)val))   // we got a packet from the hardware
	{  	  
	  nf_lock(LOCK_PKTIN);

	  int trigger = 0;
	  if((old_packets_not_in_flight <= PKTIN_OFFLOAD_THRESHOLD) &&
	     (packets_not_in_flight <= PKTIN_OFFLOAD_THRESHOLD))
	    trigger = 1;
	  
	  log("PKTIN_BACKOFF%d has %d packets not in flight, trigger %d\n", nf_tid(), packets_not_in_flight, trigger);
	  old_packets_not_in_flight = packets_not_in_flight;

	  if(trigger &&
	     (num_static_offload < PKTIN_OFFLOAD_MAX)
	     ) // offload it to dram
	    {
	      t_addr* new_start = 0;
	      int i;
#ifndef DEBUG
	      struct ioq_header *ioq = (struct ioq_header *)(val|(1<<HEADER_MEM_SEL)); // get the pointer on meta data of packet
#else
	      struct ioq_header *ioq = (struct ioq_header *)(val); // get the pointer on meta data of packet
#endif
	      unsigned int size = ntohs(ioq->byte_length);  // get size of packet
	      
	      for(i=0; i<PKTIN_OFFLOAD_MAX; i++)  // find an empty slot to put the packet
		if(!static_offload_filled[i])
		  {
		    new_start = static_offload[i];
		    break;
		  }
	      memcpy32(new_start, (t_addr*)ioq, size); // copy the packet to the slot
	      static_offload_filled[i] = 1; // mark the slot as taken
	      num_static_offload++;  // count it
	      log("PKTIN_BACKOFF%d has %d packets in store\n", nf_tid(), num_static_offload);
	      nf_unlock(LOCK_PKTIN);
	      nf_pktin_free((t_addr*)ioq); // free the packet just obtained
	      // go back to get another packet
	    }
	  else   // use immmediately the packet popped
	    {
#ifndef DEBUG
	      t_addr* addr = (t_addr*)(val|(1<<HEADER_MEM_SEL));
#else
	      t_addr* addr = (t_addr*)(val);
#endif

	      packets_not_in_flight--;
	      if(!nf_pktin_is_valid((t_addr*)val))
		log("PKTIN_BACKOFF%d gets none, %d not in flight\n", nf_tid(), packets_not_in_flight);
	      else
		log("PKTIN_BACKOFF%d gets one, %d not in flight, %d in store, ret %x\n",
		    nf_tid(), packets_not_in_flight, num_static_offload, (uint)addr);
	      nf_unlock(LOCK_PKTIN);
	      
	      return addr; // or the mem-id bit to the base address
	    }
	}
      else // didn't get a packet from the network, look in RAM
	{
	  nf_lock(LOCK_PKTIN);
	  if(num_static_offload) /// look if we have a packet in the offload buffers
	    {
	      for(i=0; i<PKTIN_OFFLOAD_MAX; i++)
		{
		  log("PKTIN_BACKOFF%d, reserve_slot %d: filled %d taken %d\n",
		      nf_tid(), i, static_offload_filled[i], static_offload_taken[i]);
		  if(static_offload_filled[i] && (!(static_offload_taken[i])))
		    {
		      val = (uint)&(static_offload[i]);
		      static_offload_taken[i] = 1;  // indicates that a thread is using that buffer
		      break;
		    }
		}
	    }
	  nf_unlock(LOCK_PKTIN);

	  if(nf_pktin_is_valid((t_addr*)val))
	    {
	      // this is a dummy write to tell the simulator that we have a packet
	      *(uint volatile*)((uint)(COMMON_TIME)) = val;
	      
	      log("PKTIN_BACKOFF%d, taking from reserve\n", nf_tid());
	    }
	  else
	    log("PKTIN_BACKOFF%d, no packet, %d not in flight, %d in reserve\n",
		nf_tid(), packets_not_in_flight, num_static_offload);
	  
	  log("PKTIN_BACKOFF%d, returning from reserve with %x\n",
	      nf_tid(), val);
	  return (t_addr*)val;  // take the offloaded packet
	}
    } // while loop
}


/*------------------------------------*/
// val is an address, 
// val is addressed in bytes so it has to be divisible by 8
void nf_iqm_pktin_free(t_addr* val)
{
  uint i, v = (uint) val;
  if(v & (1<<HEADER_MEM_SEL))
    {
      nf_pktin_free(val);

      log("PKTIN_BACKOFF%d, returning real input memory slot %x\n",
	  nf_tid(), v);

      nf_lock(LOCK_PKTIN);
      packets_not_in_flight++;
      nf_unlock(LOCK_PKTIN);
    }
  else
    {
      int found = 0;
      log("PKTIN_BACKOFF%d, returning reserve memory slot %x\n",
	  nf_tid(), v);

      // find the buffer in the cache and remove it  
      nf_lock(LOCK_PKTIN);
      for(i=0; i<PKTIN_OFFLOAD_MAX; i++)
	if(val == static_offload[i])
	  {
	    static_offload_taken[i] = 0;
	    static_offload_filled[i] = 0;
	    num_static_offload--;
	    found = 1;
	    break;
	  }      
      nf_unlock(LOCK_PKTIN);

      if(found)
	log("PKTIN_BACKOFF%d, returning reserve memory slot %x FOUND\n",
	    nf_tid(), v);
      else
	log("PKTIN_BACKOFF%d, returning reserve memory slot %x NOT FOUND\n",
	    nf_tid(), v);
    }
}
/*------------------------------------*/


