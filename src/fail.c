/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include <syslog.h>
#include <errno.h>
#include <stdarg.h>

#include "fail.h"

int
vfail (int err, const char *format, va_list list)
{
  if (format)
    vsyslog (LOG_ERR, format, list);
  errno = err;
  return -1;
}

int
fail (int err, const char *format, ...)
{
  int result;
  va_list list;

  va_start (list, format);
  result = vfail (err, format, list);
  va_end (list);

  return result;
}

int
fail_out_of_memory ()
{
  return fail (ENOMEM, "out of memory");
}
