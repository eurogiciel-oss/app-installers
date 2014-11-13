/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "context.h"
#include "step.h"
#include "step-check-wgt.h"
#include "fail.h"


static const char *list[] = {
  "config.xml"
};


static int
make (struct context *context)
{
  char buffer[PATH_MAX];
  int len, n, idx;

  assert (context->unpack_directory != NULL);

  len = (int) strlen (context->unpack_directory);
  if (len + 1 >= PATH_MAX)
    return fail (ENAMETOOLONG, "Name too long %s", context->unpack_directory);

  memcpy (buffer, context->unpack_directory, len);
  buffer[len++] = '/';

  idx = 0;
  while (idx < sizeof list / sizeof *list)
    {
      n = 1 + (int) strlen (list[idx]);
      if (len + n > PATH_MAX)
	return fail (ENAMETOOLONG, "Name too long %s/%s", context->unpack_directory, list[idx]);
      memcpy (buffer + len, list[idx], n);
      if (access (buffer, F_OK))
	return fail (ENOENT, "File not found %s", buffer);
      idx++;
    }

  return 0;
}

struct step step_check_wgt = {.make = make,.unmake = 0,.clean = 0 };
