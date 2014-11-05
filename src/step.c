/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include <errno.h>

#include "context.h"
#include "step.h"

/*
 If the given 'function' is NULL then returns 0 (no error).
 Otherwise, call the function with 'context' and propagate its result.
*/
static int
call (int (*function) (struct context *), struct context *context)
{
  return function ? function (context) : 0;
}

int
step_run (struct step **steps, struct context *context)
{
  int result, index, saved;

  /* make loop */
  result = 0;
  index = 0;
  while (!result && steps[index])
    result = call (steps[index++]->make, context);

  /* clean or unmake loop */
  saved = errno;
  if (result)
    while (index)
      call (steps[--index]->unmake, context);
  else
    while (index)
      call (steps[--index]->clean, context);
  errno = saved;

  /* end */
  return result;
}

#if !defined(NDEBUG)
struct step step_pass = {.make = 0,.unmake = 0,.clean = 0 };

static int
fails (struct context *context)
{
  errno = ECANCELED;
  return -1;
}
struct step step_fails = {.make = fails,.unmake = fails,.clean = fails };
#endif