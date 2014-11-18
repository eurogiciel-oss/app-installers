/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include <string.h>
#include <errno.h>

#include "fail.hxx"

namespace AppInstaller {

/* global value stating if throw should be done on error */
bool FailCode::should_throw = false;

/* Get the error string */
const char *FailException::what() {
  return strerror(saved_errno);
}

}

