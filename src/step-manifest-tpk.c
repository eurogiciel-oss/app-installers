/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#include "fail.h"
#include "context.h"
#include "step.h"
#include "step-manifest-tpk.h"
#include "manifest-tpk.h"


const char manifest_sub_path[] = "/info/manifest.xml";


static int
process (struct context *context)
{
  size_t length;
  char buffer[PATH_MAX];

  /* check length */
  length = strlen (context->unpack_directory);
  if (length + sizeof manifest_sub_path > sizeof buffer)
    return fail (ENAMETOOLONG, "path too long %s%s", context->unpack_directory, manifest_sub_path);

  /* path of the manifest file */
  memcpy (buffer, context->unpack_directory, length);
  memcpy (buffer + length, manifest_sub_path, sizeof manifest_sub_path);

  /* parse */
  return manifest_tpk_from_xml_file (&context->tpk_manifest, buffer);
}

static int
undo (struct context *context)
{
  return 0;
}

struct step step_manifest_tpk = {.process = process,.undo = undo,.clean = 0 };
