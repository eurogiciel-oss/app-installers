/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>

#include "fail.h"
#include "unzip.h"

int
unzip (const char *targetdir, const char *zipfile)
{
  int status;
  char *command;

  /* prepare the command */
  status =
    asprintf (&command, "unzip -n -qq -d '%s' '%s'", targetdir, zipfile);
  if (status < 0)
    return fail (ENOMEM, NULL);

  /* execute the command */
  status = system (command);
  free (command);

  /* diagnostic the command */
  switch (status)
    {
    case -1:
      return fail (errno, "Can't excute unzip");
    case 0:
    case 1:
      return 0;
    case 4:
    case 5:
    case 6:
    case 7:
      return fail (ENOMEM, NULL);
    case 9:
      return fail (ENOENT, "File to unzip doesn't exist: %s", zipfile);
    default:
      return fail (ECANCELED, "Error while unziping %s to %s", zipfile,
		   targetdir);
    }
}
