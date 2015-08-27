#include "support.h"


#warning taken from support.c
#define BUF_SIZE 1600     // has to be multiple of 8 because of 64 bit words

inline t_addr* get_cm_ptr()
{
  int tid = nf_tid();

  // ignore the lock id, we are at the top of the lock nest
  //(lockid & 0xf);

  int pos = ((tid+1) * BUF_SIZE)-4;  
  t_addr* ptr = (t_addr*)(pos | (1<<PACKETOUT_SEL));
  return ptr;
}

// since this is a transaction,
// i guess we are at the top of the lock nest 
void sw_cm_begin(int lockid)
{
  t_addr* ptr = get_cm_ptr();

  if(ptr[0] >= 2) // repeat abort detected
    {
      ptr[0] --;

      // exit critical section to be able to deschedule
      nf_unlock(lockid);

      nf_lock(LOCK_MISC);
      nf_unlock(LOCK_MISC);

      // for classifier
      relax();
      nf_lock(LOCK_MISC);
      nf_unlock(LOCK_MISC);


      // re-enter critical section to be able to deschedule
      nf_lock(lockid); 
    }
  else
    ptr[0]++;
}

// since this is a transaction,
// i guess we are at the end of the lock nest 
void sw_cm_end()
{
  t_addr* ptr = get_cm_ptr();
  ptr[0] = 0;
}
