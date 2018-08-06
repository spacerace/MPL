int lock[5];
int *comanch;

#include "stdio.h"
char *getmem();

/*          Test multi-tasking function library
 *
 *   In this test, a main task and three subtasks are created.
 *   Initally, each task identifies itself, then issues yield to
 *   allow the next task to execute.
 *
 *   main then obtains all of the locks, and enters a loop, issuing an
 *   identifying message on each pass.  At cycle 5, 10, 15 and 20,
 *   a lock is released via deq.
 *
 *   Each of the subtasks performs a limited number of iterations of the
 *   following:  enq lock, issue message, then deq lock.
 *
 *   The sequence of displays shows each of the tasks being dispatched
 *   in turn, but only when any needed lock is available.
 *
 *   Note that although parameters may not be passed to the subtasks
 *   as normal arguments, the task-id is given through the
 *   task-related data area passed via attach.
 *
 */

main()
{
  static int ids[4] = {1,2,3,4};
  int j;
  int i;

  comanch = &ids[0];
  inittask(&ids[0]);
  for (i=1;i<4; i++)
    {   /* get subtask going with 1k stack */
        if (attach(&ids[i],getmem(1024),1024)) subtask();
    }
  printf("\tMain");
  yield();                        /* give other tasks 1 shot each */
  for (i=1;i<5; i++)  enq(&lock[i]);  /* block other tasks */
  for (j=1;j<26;j++)
     {
        if (j==5)  deq(&lock[1]);
        if (j==10) deq(&lock[2]);
        if (j==15) deq(&lock[3]);
        if (j==20) deq(&lock[4]);
        printf("\tMain ");
        yield();
     }
  while (taskcnt() > 1) yield();    /* wait for completion of subtasks */
}

subtask()
{
  int i;
  int j;

  i = *comanch;
  j = i*10;
  while (j--)
    {
       enq(&lock[i]);
       printf("\t %d",i);
       deq(&lock[i]);
       yield();
    }
  stop();
}
