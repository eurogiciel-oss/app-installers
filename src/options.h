/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#pragma once

struct options
{
  char *path;			/* path */
  char *store_client_id;	/* store id */
  char *package_id;		/* package id */
  int removable;		/* removable */
};

/*
  Parse the given 'string' and fills the given option structure.

  The parsed strings are like:
	"abcde12345:StoreClientId=12345abcde.TizenStore"
	"abcde12345"
	"path=/opt/usr/apps/22VoFBmj2I/:op=install:removable=true"

  Returns 0 on success or otherwise -1 in case of failure.

  At entry, the option structure is cleared by filling it with zeros.
  At 

*/

extern int options_parse (struct options *options, const char *string);
