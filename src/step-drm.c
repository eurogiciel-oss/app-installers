/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <dlfcn.h>
#include <syslog.h>
#include <errno.h>

#include "context.h"
#include "step.h"
#include "step-drm.h"

//#define DONT_DISABLE_LEGACY_DRM_OEM

#if !defined(DONT_DISABLE_LEGACY_DRM_OEM)

struct step step_drm = {.make = 0,.unmake = 0,.clean = 0 };

#else

static const char library_path[] = "/usr/lib/libdrm-service-core-tizen.so";
static const char name_is_drm_file[] = "drm_oem_sapps_is_drm_file";
static const char name_decrypt_package[] = "drm_oem_sapps_decrypt_package";

static void *handle = NULL;
static int (*is_drm_file) (const char *, int) = NULL;
static int (*decrypt_package) (const char *, int, const char *, int) = NULL;


static void
uninit ()
{
  if (handle != NULL)
    {
      dlclose (handle);
      handle = NULL;
      is_drm_file = NULL;
      decrypt_package = NULL;
    }
}

static int
init ()
{
  if (handle != NULL)
    return 0;
  handle = dlopen (library_path, RTLD_LAZY | RTLD_GLOBAL);
  if (handle == NULL)
    return 0;
  is_drm_file = dlsym (handle, name_is_drm_file);
  decrypt_package = dlsym (handle, name_decrypt_package);
  if (is_drm_file != NULL && decrypt_package != NULL)
    return 0;
  syslog (LOG_ERR, "Symbol(s) not found%s%s",
	  is_drm_file == NULL ? name_is_drm_file : "",
	  decrypt_package == NULL ? name_decrypt_package : "");
  uninit ();
  errno = ECANCELED;
  return -1;
}

static int
clean (struct context *context)
{
  char *decrypted, *original;

  original = context->package_path_drm_save;
  if (original != NULL)
    {
      decrypted = context->package_path;
      context->package_path = original;
      context->package_path_drm_save = NULL;
      unlink (decrypted);
      free (decrypted);
    }
  uninit ();
}

static int
make (struct context *context)
{
  int len;
  char *decrypted, *original;
  static const char extension[] = ".d.tpk";

  assert (context->package_path != NULL);
  assert (context->package_path_drm_save == NULL);

  if (init ())
    return -1;

  if (handle == NULL)
    return 0;

  original = context->package_path;
  len = (int) strlen (original);

  if (is_drm_file (original, len) != 1)
    return 0;

  if (asprintf (&decrypted, "%s%s", original, extension) < 0)
    {
      uninit ();
      errno = ENOMEM;
      return -1;
    }

  if (decrypt_package (original, len, decrypted, strlen (decrypted)) != 1)
    {
      unlink (decrypted);
      free (decrypted);
      errno = ECANCELED;
      return -1;
    }

  context->package_path = decrypted;
  context->package_path_drm_save = original;
  return 0;
}

struct step step_drm = {.make = make,.unmake = clean,.clean = clean };

#endif
