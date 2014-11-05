/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#pragma once

/* context is defined somewhere */
struct context;

/*
  A step is made of 3 functions that can be defined or NULL.

  These functions all have the same signature: they accept
  a pointer to a context of installation and they return
  an integer value stating the execution issue. If the returned
  code is 0 then the excution finished succefuly. Otherwise,
  the returned code should be set to -1 with errno set to some
  meaningful value.

  The functions are:
    - make     make the installation step
    - unmake   revert the installation step
    - clean    remove any temporary data
*/
struct step
{
  int (*make) (struct context *);
  int (*unmake) (struct context *);
  int (*clean) (struct context *);
};

/*
 Run the given 'steps' by calling the callback functions "make" of
 the steps with 'context' in order until the end (indicated
 by a NULL step pointer) is reached or a callback function "make"
 returned an error.

 On success (all steps are made without error), the callback functions
 "clean" of the steps are called in the reverse order and a value of
 0 is returned.

 On error, the callback functions "unmake" are called in the reverse
 order and the original error is returned.
*/
int step_run (struct step **steps, struct context *context);

#if !defined(NDEBUG)
/*
 This predefined steps 'step_pass' and 'step_fails' can be used
 for debugging.
*/
extern struct step step_pass, step_fails;
#endif