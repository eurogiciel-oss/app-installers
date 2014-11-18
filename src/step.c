/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include <errno.h>
#include <assert.h>

#include "context.h"
#include "step.h"

/*
 If the given 'function' is NULL then returns 0 (no error).
 Otherwise, call the function with 'data' and propagate its result.
*/
static int
call (int (*function) (data *), void *data)
{
  return function ? function (data) : 0;
}

int
step_run (struct step **steps, int count)
{
  int result, index, saved;

  assert(steps != NULL);
  assert(count >= 0);

  /* process loop */
  result = 0;
  index = 0;
  while (!result && index < count)
    {
      result = call (steps[index]->process, steps[index]->data);
      index++;
    }

  /* clean or undo loop */
  saved = errno;
  while (index)
    {
      index--;
      call (result ? steps[index]->undo : steps[index]->clean, steps[index]->data);
    }
  errno = saved;

  /* end */
  return result;
}

#if !defined(NDEBUG)
struct step step_pass = {.process = NULL, .undo = NULL, .clean = NULL, .data = NULL };

static int
fails (void * data)
{
  errno = ECANCELED;
  return -1;
}
struct step step_fails = {.process = fails,.undo = fails,.clean = fails, .data = NULL };
#endif
