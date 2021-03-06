#define MAIN_SUPPORT
#include "support.h"
#include <stdarg.h>

#ifdef DEBUG
#include <malloc.h>
#include <stdio.h>
#endif

// 'volatile' must be left of the pointer star
//http://en.wikipedia.org/wiki/Const_correctness
/*------------------------------------*/

#define NUM_OUTPUT_BUFS 10
#define BUF_SIZE 1600     // has to be multiple of 8 because of 64 bit words

// has to be initialized to 0 for nf_stall_a_bit() to work
static volatile int init_flag=0; // a bitfield. bit is 1 if the buffer is free

int po_buffers_free=0; // a bitfield. bit is 1 if the buffer is free
int error_condition = 0;

void nf_pktout_init()
{
  int i;
  nf_lock(LOCK_PO_MEM);
  po_buffers_free = 0;

  for(i=0; i<NUM_OUTPUT_BUFS; i++)
    {
      po_buffers_free |= (1 << i);
    }
  // Clear the first bit. The first call to nf_pktout_send will collect
  // the first buffer, so we must avoid allocating it before that.
  po_buffers_free &= ~1;
  nf_unlock(LOCK_PO_MEM);
}

/* Ugly code to find the index of the set bit in the 10
least-signifcant bits of a value. This is faster than 
looping over them. 
Assumes there is a bit set and returns its index, 0-9.
*/
static int bitscan10(int value) {
  int ret = 0;
  if (value & 0x3E0) {
    // 1 is in hi 5 bits
    value >>= 5;
    ret = 5;
  }
  if (value & 0x18) {
    // 1 is in hi 2 bits
    value >>= 3;
    ret += 3;
  } else if (value == 4) {
    return ret + 2;
  } 
  
  // value = 2 or 1
  return ret + (value - 1);
}

t_addr* nf_pktout_alloc(uint size)
{
#ifndef DEBUG
  int before, after=0;
  t_addr* ret = 0;

  /* assume this doesn't happen often
     this synchronized data is outside a critical section, which complicates
     transactional memory */
  //if (po_buffers_free == 0) {
  //  return 0;
  //}

  nf_lock(LOCK_PO_MEM);
  before = po_buffers_free;
  if (before) {
    // flip right most bit
    after = before & (before - 1);
    po_buffers_free = after;
  }
  nf_unlock(LOCK_PO_MEM);

  if (before) {
    //int i;
    //simprintf("buffer before 0x%X after 0x%X one bit 0x%X\n", before, after, after ^ before);
    // determine which bit we flipped
    after = after ^ before;
    
    /*for(i=0; i<NUM_OUTPUT_BUFS; i++)
      {
        if( after & (1 << i) )
          {
            ret = (t_addr*)((i*BUF_SIZE) | (1<<PACKETOUT_SEL));
            //simprintf("Allocate buffer %d = 0x%x\n", i, ret);
            break;
          }
          }*/
    return (t_addr*)((bitscan10(after)*BUF_SIZE) | (1<<PACKETOUT_SEL));
    
	/*
    // count buffers
    i = 0;
    while (before) {
      before = before & (before - 1);
      i++;
    }
    
    simprintf("Allocated po_buffer %d still free\n", i - 1);
	*/
  }
  return ret;
#else
  t_addr* ptr = (t_addr*) malloc(size);
  log("allocated %p\n", ptr);
  return ptr;
#endif
}

static void collect_sent(t_addr* addr)
{
#ifndef DEBUG
  // figure out the index of the buffer from the address
  uint addr_val = (uint)addr & ~(1<<PACKETOUT_SEL);
  int i = 1 << (addr_val / BUF_SIZE);
  //simprintf("Free buffer %d = 0x%x\n", addr_val / BUF_SIZE, addr);
  nf_lock(LOCK_PO_MEM);

#if 0
  int cnt=0;
  // stop here if the buffer was already free
  while(  (po_buffers_free & i) == i)
    {
      error_condition = 1;
      if(cnt&1)  // FOR DEBUGGING
	addr_val++; // FOR DEBUGGING
      else // FOR DEBUGGING
	addr_val--; // FOR DEBUGGING
      cnt++; // FOR DEBUGGING
    }
  if(cnt) po_buffers_free += addr_val; // FOR DEBUGGING
#endif

  po_buffers_free |= i;

  nf_unlock(LOCK_PO_MEM);
#else
  log("freeing %p\n", addr);
  free(addr);
#endif
}

void nf_pktout_free(t_addr* addr) {
  collect_sent(addr);
}


#ifdef DEBUG
static char* send_setup_start_addr=0;
static char* send_setup_end_addr=0;
void nf_pktout_send_setup(char* start_addr, char* end_addr) {
  send_setup_start_addr = start_addr;
  send_setup_end_addr = end_addr;
}
void nf_pktout_send_schedule(unsigned scheduled_time) {
}
void nf_pktout_send_finish() {
  nf_pktout_send(send_setup_start_addr, send_setup_end_addr);
}
#else

/* THIS WILL NOT WORK IN TM MODE BECAUSE OF THE LACK OF 
   HAZARD BITS FOR THIS MEM_MAPPED OPS */
void nf_pktout_send_setup(char* start_addr, char* end_addr) {
  uint ctrl = calc_ctrl(start_addr, end_addr);
  end_addr--;

  *(volatile uint*)CTRL_START = (uint)start_addr;
  *(volatile uint*)CTRL_END = (uint)end_addr;
  *(volatile uint*)CTRL_END_W = ctrl;
}
void nf_pktout_send_schedule(unsigned scheduled_time) {
  *(volatile uint*)SEND_TIME_TRIG = scheduled_time;
}
void nf_pktout_send_finish() {
  // the value is volatile and has type t_addr*
  t_addr* last_packet = *(t_addr* volatile *)SEND_OUT_W;
  collect_sent(last_packet);
}

#endif

//volatile int myerror = 0;
void nf_pktout_send(char* start_addr, char* end_addr)
{
#ifndef DEBUG
  uint   volatile *loc1, *loc2, *loc3, *loc4;
  t_addr* last_packet;
  uint ctrl;
  
  while((end_addr <= start_addr) || error_condition)
    {
      error_condition = 1;
      uint   volatile *loc10 = (uint*)((uint)(COMMON_TIME));
      *loc10 = 0;
      nf_lock(2);	nf_lock(2);
    }

  ctrl = calc_ctrl(start_addr, end_addr);
  end_addr--;

  loc1 = (uint*)(/*(uint)packetout_mem |*/ (uint)(CTRL_START));
  loc2 = (uint*)(/*(uint)packetout_mem |*/ (uint)(CTRL_END));
  loc3 = (uint*)(/*(uint)packetout_mem |*/ (uint)(CTRL_END_W));
  loc4 = (uint*)(/*(uint)packetout_mem |*/ (uint)(SEND_OUT_W));

  // tighly scheduled to save time
  nf_lock(SENDING_LOCK);  // has to be a critical section for scheduler
  *loc1 = (uint)start_addr;
  *loc2 = (uint)end_addr;
  *loc3 = ctrl;
  // this should be the only memory mapped read to correspond to the hardware
  last_packet = (t_addr*)((*loc4) /*| (uint)(1<<PACKETOUT_SEL)*/);
  nf_unlock(SENDING_LOCK);
  collect_sent(last_packet);
#else
#if 0
  int len = end_addr - start_addr;
  int i,j,k;
  log("outgoing length is %d\n", len);
  for(i=0; i<len; )
    {
      for(j=0; (j<8)&&(i<len);j++)
        {
          for(k=0; (k<2)&&(i<len);(k++,i++))
            log("%02x", (((int)(start_addr[i])) & 0xff));
          log(" ");
        }
      log("\n");
    } 
#else	
	extern void sw_pktout_send(char*, char*);
	sw_pktout_send(start_addr, end_addr);
#endif
	//collect_sent(start_addr);
#endif
}

#ifdef DEBUG
// the non DEBUG implementation of nf_time() is in support.h
static int mytime=0;
uint nf_time()
{
  return mytime++;
}
#endif

/* oct6
static int mytime[MAX_THREAD]={0,0,0,0,0,0,0,0};
uint nf_time()
{
  int tid = nf_tid();
  mytime[tid] += 10;
  return mytime[tid];
}
*/

// only for initialization purposes
int nf_stall_a_bit()
{ 
  int i=0;
  /*  while(1)
      {
      nf_lock(LOCK_PO_MEM);
      if(po_buffers_free != 0)
      break;
      nf_unlock(LOCK_PO_MEM);
      }
      nf_unlock(LOCK_PO_MEM);
  */
  while(init_flag == 0) {
    i ++;
    relax();
  }

  return i;
}


void init_stack()
{
  int t = nf_tid();

  //  this code doesn't handle branches well
  int diff = -1*t*STACK_IN_BYTES - STACK_SKIP - t*(DCACHE/NUM_CPU/THREADS_PER_CPU);

#ifndef DEBUG
  asm(".set    noreorder");
  asm("lw      $31,16($29)\n\t"
      "addu    $29,$29,%0\n\t"
      "jr      $31"
      :
      : "r" (diff) ); 
  asm(".set    reorder");
#endif

}




/*
  Default initialization of the input memory
  to hold 10 packets. To choose some other
  division, simply don't call this and call
  nf_pktin_free directly.
 */
void nf_pktin_init() {
//  int i=0;
//  for (i = 0; i < 10; i++) {
//    nf_pktin_free((t_addr*) (1600 * i));
//  }    
//  nf_pktin_free((t_addr*)0);
  nf_pktin_free((t_addr*)1600);
  nf_pktin_free((t_addr*)3200);
  nf_pktin_free((t_addr*)4800);
  nf_pktin_free((t_addr*)6400);
  nf_pktin_free((t_addr*)8000);
  nf_pktin_free((t_addr*)9600);
  nf_pktin_free((t_addr*)11200);
  nf_pktin_free((t_addr*)12800);
  nf_pktin_free((t_addr*)14400);
  init_flag = 1; // required to exit stall_a_bit on the other threads
}


#ifndef CONTEXT_SW //def CONTEXT_SIM 

void simprintf(char *frmt, ...) {
  va_list ap;
  uint volatile* printf_addr = (uint*)PRINTF_W;

  *(uint*)printf_addr = (uint) frmt;
  
  // for each %arg in the format string, write the arg's
  // value to the simulator
  va_start(ap, frmt);
  while (*frmt) {
    if (*frmt == '%') {
      // make sure the next char isn't a '%' too
      if (*(frmt + 1) == '%') {
        // skip over it
        frmt++;
      } else {
        *(uint*)printf_addr = va_arg(ap, uint);
      }
    }
    frmt++;
  }
  va_end(ap);

  // one more dummy write to finish the printf
  *(uint*)printf_addr = 0;
}

#endif //NEW

#ifndef CONTEXT_NF

// Print contents of packet. Right now this just prints hex values.
// Could be extended to optionally parse the packet too.
void print_pkt(char *pkt, int len) {
  int i,j,k;
  int p = 0;
  len += 8;
  //myprintf("Packet of size %d:\n", len);
  for(i=0; i<len; ) {
    for(j=0; (j<8)&&(i<len);j++) {
      for(k=0; (k<2)&&(i<len);(k++,i++)) {
        int t = pkt[p++];
        simprintf("%02x", 0xFF & t);
      }
      simprintf(" ");
    }
    simprintf("\n");
  }
}
#endif

