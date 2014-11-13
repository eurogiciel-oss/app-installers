/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <assert.h>
#include <errno.h>
#include <stdarg.h>

#include "options.h"
#include "fail.h"

enum keys
{
  no_key,
  key_removable,
  key_path,
  key_store_client_id,
  key_package_id
};

static int
myfail (struct options *options, int err, const char *message, ...)
{
  va_list list;
  int result;

  free (options->path);
  free (options->store_client_id);
  free (options->package_id);
  memset (options, 0, sizeof *options);

  va_start (list, message);
  result = vfail (err, message, list);
  va_end (list);

  return result;
}

int
options_parse (struct options *options, const char *string)
{
  const char *head, *iter, *value;
  char *data;
  int length, mask;
  enum keys type;

  assert (string);

  memset (options, 0, sizeof *options);
  mask = 0;

  /* parse the string option */
  for (iter = string; *iter; iter++)
    {
      head = iter;
      while (*iter && *iter != ':' && *iter != '=')
	iter++;
      if (*iter == '=')
	{
	  value = ++iter;
	  while (*iter && *iter != ':')
	    iter++;
	  type = no_key;
	  switch (value - head)
	    {
	    case 5:
	      if (!strncasecmp (head, "path", 4))
		type = key_path;
	      break;
	    case 10:
	      if (!strncasecmp (head, "removable", 9))
		type = key_removable;
	      break;
	    case 14:
	      if (!strncasecmp (head, "storeclientid", 13))
		type = key_store_client_id;
	      break;
	    }
	  if (type == no_key)
	    return myfail (options, EINVAL,
			   "Error while parsing option string: invalid key %.*s", (int) (value - head) - 1, head);
	}
      else
	{
	  type = key_package_id;
	  value = head;
	}

      if ((mask & (1 << type)) != 0)
	return myfail (options, EINVAL, "Error while parsing option string: duplication of a key");

      length = (int) (iter - value);
      if (type == key_removable)
	{
	  if ((length == 1 && *value == '0') || (length == 5 && !strncasecmp (head, "false", 5)))
	    options->removable = 0;
	  else if ((length == 1 && *value == '1') || (length == 4 && !strncasecmp (head, "true", 5)))
	    options->removable = 1;
	  else
	    return myfail (options, EINVAL,
			   "Error while parsing option string: wrong boolean value %.*s", length, value);
	}
      else
	{
	  data = strndup (value, length);
	  if (data == NULL)
	    return myfail (options, ENOMEM, NULL);
	  switch (type)
	    {
	    case key_path:
	      options->path = data;
	      break;
	    case key_store_client_id:
	      options->store_client_id = data;
	      break;
	    case key_package_id:
	      options->package_id = data;
	      break;
	    default:
	      assert (0 != 0);
	    }
	}
      mask |= 1 << type;
    }
  return 0;
}
