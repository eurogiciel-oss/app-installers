/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

#include <pkgmgr/pkgmgr_parser.h>

#include "context.h"
#include "step.h"
#include "step-manifest.h"
#include "utils.h"

static int
make (struct context *context)
{
  return 0;
}

static int
unmake (struct context *context)
{
  return 0;
}


struct step step_manifest = {.make = make,.unmake = unmake,.clean = 0 };