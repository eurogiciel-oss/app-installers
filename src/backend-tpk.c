/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <pkgmgr/pkgmgr_installer.h>

#include "fail.h"
#include "options.h"
#include "context.h"
#include "step.h"
#include "step-unpack.h"
#include "step-check-signature.h"
#include "step-check-tpk.h"
#include "step-manifest-tpk.h"

static pkgmgr_installer *pi = 0;
static struct options options;

struct context context;

static const struct step *install_steps[] = {
  &step_unpack,
  &step_check_signature,
  &step_check_tpk,
  &step_manifest_tpk
};


/*
 Based on a directory 'path', the function is returning
 a freshly allocated string being the packageid part
 extracted from path.
*/
static char *
get_package_id_from_package_path (const char *path)
{
  char *result;
  struct stat s;
  int status, i, j;

  status = stat (path, &s);
  if (status)
    {
      fail (errno, "Can't stat file %s: %m", path);
      return NULL;
    }

  result = NULL;
  if (S_ISDIR (s.st_mode))
    {
      for (i = j = 0; path[i]; i++)
	if (i && path[i - 1] == '/')
	  j = i;
      while (i > j && path[i - 1] == '/')
	i--;
      if (i == j)
	fail (EINVAL, "Empty package id from path %s", path);
      else
	{
	  result = strndup (path + j, i - j);
	  if (result == NULL)
	    errno = ENOMEM;
	}
    }
  else
    fail (ENOTSUP, "Unsupported package mode for path %s", path);
  return result;
}

/*
 Installing the package.
*/
static int
install ()
{
  int status;
  const char *path, *optionstr, *store_client_id;

  assert (pi);

  path = pkgmgr_installer_get_request_info (pi);
  optionstr = pkgmgr_installer_get_optional_data (pi);
  store_client_id = pkgmgr_installer_get_caller_pkgid (pi);

  status = options_parse (&options, optionstr ? optionstr : "");
  if (status)
    return errno;

  if (!options.package_id)
    {
      options.package_id = get_package_id_from_package_path (path);
      if (options.package_id == NULL)
	return ENOMEM;
    }

  /* TODO */
  status = step_run (install_steps, sizeof install_steps / sizeof * install_steps);

  return 0;
}


static int
uninstall ()
{
  return 0;
}


static int
reinstall ()
{
  return 0;
}


static int
move ()
{
  return 0;
}


int
main (int argc, char **argv)
{
  int result;

  /* get request data */
  pi = pkgmgr_installer_new ();
  if (!pi)
    return ENOMEM;
  result = pkgmgr_installer_receive_request (pi, argc, argv);
  if (result)
    {
      pkgmgr_installer_free (pi);
      return -result;
    }

  /* treat the request */
  switch (pkgmgr_installer_get_request_type (pi))
    {
    case PKGMGR_REQ_INSTALL:
      result = install ();
      break;

    case PKGMGR_REQ_UNINSTALL:
      result = uninstall ();
      break;

    case PKGMGR_REQ_REINSTALL:
      result = reinstall ();
      break;

    case PKGMGR_REQ_MOVE:
      result = move ();
      break;

    case PKGMGR_REQ_CLEAR:
    case PKGMGR_REQ_RECOVER:
    case PKGMGR_REQ_GETSIZE:
    case PKGMGR_REQ_UPGRADE:
    case PKGMGR_REQ_SMACK:
    default:
      /* unsupported operation */
      result = EINVAL;
      break;
    }
  pkgmgr_installer_free (pi);
  return result;
}
