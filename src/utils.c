/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */
#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

#include "utils.h"

ssize_t
utils_read (int fd, void *buf, size_t count)
{
  ssize_t result;

  do
    {
      result = read (fd, buf, count);
    }
  while (result < 0 && errno == EINTR);
  return result;
}

ssize_t
utils_write (int fd, const void *buf, size_t count)
{
  ssize_t result;

  do
    {
      result = write (fd, buf, count);
    }
  while (result < 0 && errno == EINTR);
  return result;
}
