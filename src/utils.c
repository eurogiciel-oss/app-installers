/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */
#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <limits.h>

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>


int
utils_unzip (const char *targetdir, const char *zipfile)
{
  int status;
  char *command;

  /* prepare the command */
  status =
    asprintf (&command, "unzip -n -qq -d '%s' '%s'", targetdir, zipfile);
  if (status < 0)
    {
      errno = ENOMEM;
      return -1;
    }

  /* execute the command */
  status = system (command);
  free (command);

  /* diagnostic the command */
  switch (status)
    {
    case -1:
      syslog (LOG_ERR, "Can't excute unzip");
      break;
    case 0:
    case 1:
      return 0;
    case 4:
    case 5:
    case 6:
    case 7:
      errno = ENOMEM;
      break;
    case 9:
      syslog (LOG_ERR, "File to unzip doesn't exist: %s", zipfile);
      errno = ENOENT;
      break;
    default:
      errno = ECANCELED;
      syslog (LOG_ERR, "Error while unziping %s to %s", zipfile, targetdir);
    }
  return -1;
}




static int
_remove_directory_content (char path[PATH_MAX], int length)
{
  DIR *dir;
  struct dirent *ent;
  int n;

  if (length + 1 >= PATH_MAX)
    {
      errno = ENAMETOOLONG;
      return -1;
    }

  dir = opendir (path);
  if (!dir)
    return -1;

  path[length++] = '/';
  path[length] = 0;

  errno = 0;
  ent = readdir (dir);
  while (ent != NULL)
    {
      n = (int) strlen (ent->d_name);
      assert (n > 0);
      if (n > 2 || ent->d_name[0] != '.' || ent->d_name[n - 1] != '.')
	{
	  if (length + n >= PATH_MAX)
	    {
	      errno = ENAMETOOLONG;
	      break;
	    }
	  memcpy (path + length, ent->d_name, n + 1);
#ifdef _DIRENT_HAVE_D_TYPE
	  if (ent->d_type != DT_DIR)
	    {
	      if (unlink (path))
		break;
	    }
	  else
	    {
	      if (_remove_directory_content (path, length + n)
		  || rmdir (path))
		break;
	    }
#else
	  if (unlink (path) || errno != EISDIR
	      || _remove_directory_content (path, length + n) || rmdir (path))
	    break;
#endif
	}
      errno = 0;
      ent = readdir (dir);
    }
  closedir (dir);
  path[length - 1] = 0;
  return errno ? -1 : 0;
}

int
utils_remove_directory_content (const char *path)
{
  char buffer[PATH_MAX];
  int length;

  length = (int) strlen (path);
  if (length >= PATH_MAX)
    {
      errno = EINVAL;
      return -1;
    }
  memcpy (buffer, path, length + 1);
  return _remove_directory_content (buffer, length);
}

int
utils_remove_directory (const char *path, int force)
{
  if (force && utils_remove_directory_content (path))
    return -1;
  return rmdir (path);
}

int
utils_remove_any (const char *path)
{
  return unlink (path) || errno != EISDIR
    || utils_remove_directory (path, 1) ? -1 : 0;
}


int
utils_copy_file (const char *dest, const char *src, int force)
{
  int fdfrom, fdto, result;
  ssize_t length;
  size_t count;
  struct stat s;

  result = -1;
  fdfrom = open (src, O_RDONLY);
  if (fdfrom >= 0)
    {
      if (!fstat (fdfrom, &s))
	{
	  fdto = open (src, O_WRONLY | O_TRUNC | O_CREAT, s.st_mode | 0200);
	  if (fdto >= 0)
	    {
	      for (;;)
		{
		  count = (~(size_t) 0) >> 1;
		  if ((off_t) count > s.st_size)
		    count = (size_t) s.st_size;
		  length = sendfile (fdto, fdfrom, NULL, count);
		  if (length < 0)
		    break;
		  s.st_size -= (off_t) length;
		  if (s.st_size == 0)
		    {
		      result = 0;
		      break;
		    }
		}
	    }
	  close (fdto);
	  if (result)
	    unlink (dest);
	}
      close (fdfrom);
    }
  return result;
}


static int
_copy_directory (char dest[PATH_MAX], int destlen, char src[PATH_MAX],
		 int srclen, int force)
{
  DIR *dir;
  struct dirent *ent;
  int n;
#ifdef _DIRENT_HAVE_D_TYPE
  int sts;
#else
  struct stat st;
#endif

  if (srclen + 1 >= PATH_MAX || destlen + 1 > PATH_MAX)
    {
      errno = ENAMETOOLONG;
      return -1;
    }

  dir = opendir (src);
  if (!dir)
    return -1;

  src[srclen++] = '/';
  src[srclen] = 0;
  dest[destlen++] = '/';
  dest[destlen] = 0;

  errno = 0;
  ent = readdir (dir);
  while (ent != NULL)
    {
      n = (int) strlen (ent->d_name);
      assert (n > 0);
      if (n > 2 || ent->d_name[0] != '.' || ent->d_name[n - 1] != '.')
	{
	  if (srclen + n >= PATH_MAX || destlen + n >= PATH_MAX)
	    {
	      errno = ENAMETOOLONG;
	      break;
	    }
	  memcpy (src + srclen, ent->d_name, n + 1);
	  memcpy (dest + destlen, ent->d_name, n + 1);
#ifdef _DIRENT_HAVE_D_TYPE
	  switch (ent->d_type)
	    {
	    case DT_DIR:
	      sts = mkdir (dest, 0777);
	      if (!sts || errno == EEXIST)
		{
		  sts =
		    _copy_directory (dest, destlen + n, src, srclen + n,
				     force);
		}
	      break;
	    case DT_REG:
	      sts = utils_copy_file (dest, src, force);
	      break;
	    case DT_LNK:
	    default:
	      sts = 0;
	      break;
	    }
	  if (sts)
	    break;
#else
	  if (lstat (src, &st))
	    break;
	  if (S_ISDIR (st.st_mode))
	    {
	      if (mkdir (dest, 0777) && errno != EEXIST)
		break;
	      if (_copy_directory (dest, destlen + n, src, srclen + n, force))
		break;
	    }
	  else if (S_ISREG (st.st_mode))
	    {
	      if (utils_copy_file (dest, src, force))
		break;
	    }
#endif
	}
      errno = 0;
      ent = readdir (dir);
    }
  closedir (dir);
  src[srclen - 1] = 0;
  dest[destlen - 1] = 0;
  return errno ? -1 : 0;
}


int
utils_copy_directory (const char *dest, const char *src, int force)
{
  char bufdest[PATH_MAX], bufsrc[PATH_MAX];
  int lendest, lensrc;

  lendest = (int) strlen (dest);
  lensrc = (int) strlen (src);
  if (lendest >= PATH_MAX || lensrc >= PATH_MAX)
    {
      errno = EINVAL;
      return -1;
    }
  memcpy (bufdest, dest, lendest + 1);
  return _copy_directory (bufdest, lendest, bufsrc, lensrc, force);
}










