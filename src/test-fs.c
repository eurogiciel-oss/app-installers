#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "fs.h"

char *files[] = {
  "toto/a",
  "toto/b",
  "toto/c",
  "toto/d",
  "toto/e",
  "toto/f",
  "toto/toto/x",
  "toto/toto/y",
  "toto/toto/z",
  "toto/titi/x",
  "toto/titi/y",
  "toto/titi/z",
  NULL
};

enum fs_action
px (const struct fs_entry *entry)
{
  printf ("entry [%d] %s %s %s %s \n", entry->type, entry->path,
	  entry->relpath, entry->name, entry->base);
  return 0;
}

int
main ()
{
  int i;
  char **f = files;
  fs_remove_any ("toto");
  fs_remove_any ("tata");
  while (*f)
    {
      char *n = strdup (*f++);
      char *e = strrchr (n, '/');
      *e = 0;
      mkdir (n, 0777);
      *e = '/';
      i = creat (n, 0777);
      write (i, n, strlen (n));
      close (i);
      free (n);
    }
  fs_explore ("toto", wants_all, px, NULL);
  fs_copy_directory ("tata", "toto", 1);
  fs_remove_any ("toto");
  fs_mkdir ("tata///tata/tata//titi", 0777);
  fs_explore ("tata", wants_any_pre, px, NULL);
  return 0;
}
