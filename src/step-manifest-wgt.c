/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

/*
#include <pkgmgr/pkgmgr_parser.h>
*/

#include "context.h"
#include "step.h"
#include "step-manifest-wgt.h"

static int
process (void * data)
{
  return 0;
}

static int
undo (void * data)
{
  return 0;
}


struct step step_manifest_wgt = {.process = process,.undo = undo,.clean = 0, .data = 0 };
