/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#pragma once

/*
  A step is made of 3 functions (that can be defined or NULL)
  and one data pointer.

  The functions are:
    - process  process the installation step
    - undo     undo the installation step after failure
    - clean    remove temporary data of the step after success

  These functions all have the same signature: they accept
  a pointer to something and they return an integer value stating
  the execution issue.

  At the execution time, this functions if not NULL, will receive
  the 'data' pointer as first argument.

  The returned code of 0 indicates a succeful execution.
  Otherwise, the returned code should be set to -1 with errno set
  to some meaningful value.
*/
struct step
{
  int (*process) (void *);
  int (*undo) (void *);
  int (*clean) (void *);
  void *data;
};

/*
 Run the given 'steps' by calling in order the callback functions "process"
 of the steps with their associated data until the end (indicated
 by a 'count' of steps) is reached or a callback function "process"
 returned an error.

 On success (all steps processed without error), the callback functions
 "clean" of the steps are called in the reverse order and a value of
 0 is returned.

 On error, the callback functions "undo" are called in the reverse
 order, begining from the last processed step (that failed), and
 the original error is returned.
*/
int step_run (struct step **steps, int count);

#if !defined(NDEBUG)
/*
 This predefined steps 'step_pass' and 'step_fails' can be used
 for debugging.
*/
extern struct step step_pass, step_fails;
#endif
