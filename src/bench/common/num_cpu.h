#ifndef NUM_CPU_H
#define NUM_CPU_H

/* This header is included in both C and assembler code, so be
   careful. */

/* Default to 2 CPUs */
#ifndef NUM_CPU
#define NUM_CPU 2
#endif

#if NUM_CPU != 1 && NUM_CPU != 2 && NUM_CPU != 4 && NUM_CPU != 8
#error NUM_CPU must be either 1,2,4,8 or the nf2.S needs to be updated
#endif        

#ifndef THREAD_PER_CPU
#define THREAD_PER_CPU 4
#endif

#ifndef MAX_THREAD
#define MAX_THREAD (NUM_CPU*THREADS_PER_CPU)
#endif

#endif
