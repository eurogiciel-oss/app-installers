/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "context.h"
#include "step.h"
#include "step-unpack.h"
#include "fail.h"
#include "unzip.h"
#include "fs.h"

#define MAX_TRIAL_COUNT  5

static int
process (void * data)
{
  char *name;
  int id, status, nr;

  assert (context.package_id != NULL);
  assert (context.package_path != NULL);
  assert (!access (context.package_path, F_OK));

  nr = 0;
  id = (int) getpid ();
  for (;;)
    {
      status = asprintf (&name, "/tmp/.app.installers.%s.%d", context.package_id, id);
      if (status < 0)
	return fail (ENOMEM, NULL);
      status = mkdir (name, 0700);
      if (!status)
	{
	  context.unpack_directory = name;
	  /* TODO: add checks, i.e. not splitted, not crypted */
	  return unzip (context.unpack_directory, context.package_path);
	}
      free (name);
      if (errno != EEXIST)
	return fail (errno, "Can't create temporary directory: %m");
      if (++nr >= MAX_TRIAL_COUNT)
	return fail (EEXIST, "Can't create temporary directory: maximum trial count reached");
      id++;
    }
}

static int
clean (void * data)
{
  int status;

  if (context.unpack_directory)
    {
      assert (!access (context.unpack_directory, W_OK));
      status = fs_remove_directory (context.unpack_directory, 1);
      free (context.unpack_directory);
      context.unpack_directory = NULL;
    }
  else
    status = 0;

  return status;
}


struct step step_unpack = {.process = process,.undo = clean,.clean = clean, .data = 0 };
