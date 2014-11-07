/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */
#pragma once

/* Removes any file in the directory of 'path' */
int fs_remove_directory_content (const char *path);

/* Removes the directory of 'path'. If 'force'==0 the operation
 fails if the directory has an other content than "." and "..".
 If 'force'!=0 then the content of the directory is removed
 before. */
int fs_remove_directory (const char *path, int force);

/* Removes any file or directory of 'path'. If 'path' is a
 directory, its content is removed. */
int fs_remove_any (const char *path);

/* Copies the file of path 'src' in the file of path 'dest'.
 If 'force'==0 no copy is done if 'dest' already exists.
 If 'force'!=0 the file 'dest' is overwriten if it exists. */
int fs_copy_file (const char *dest, const char *src, int force);

/* Copies recursively the content of the directory of path 'src'
 to the directory of path 'dest' ('dest' is created if it doesn't exist).
 If 'force'==0 existing files are not overwriten and an error if returned.
 If 'force'==1 existing files are overwriten. SEE: fs_set_mkdir_mode. */
int fs_copy_directory (const char *dest, const char *src, int force);

/* Create the directory of 'path' and if needed its parents
 with the current creation mode. Don't returns error if the
 directory already exists. SEE: fs_set_mkdir_mode. */
int fs_mkdir (const char *path);

/* Sete the default mode for creating directories.
 Returns the previous one. CAUTION: not thread safe. */
mode_t fs_set_mkdir_mode(mode_t mode);

/* types of entries */
enum fs_type
{
  type_regular,			/* regular file */
  type_directory_pre,		/* directory in pre order */
  type_directory_post,		/* directory in post order */
  type_symlink,			/* symbolic link */
  type_others			/* any other entry type */
};

/* type of entries expected */
enum fs_wants
{
  wants_regular = 1,		/* regular files */
  wants_directory_pre = 2,	/* directory before exploring it */
  wants_directory_post = 4,	/* directory after exploring it */
  wants_symlink = 8,		/* symbolic link */
  wants_others = 16,		/* other files */
  wants_error_on_unwanted = 32,	/* raise an error on unwanted entries (set errno=ECANCELED) */

  /* some common combinations */
  wants_all =
    wants_regular | wants_symlink | wants_others | wants_directory_pre |
    wants_directory_post,
  wants_any_pre =
    wants_regular | wants_symlink | wants_others | wants_directory_pre,
  wants_any_post =
    wants_regular | wants_symlink | wants_others | wants_directory_post
};

/* type of action to perform after exploring an entry */
enum fs_action
{
  action_stop_error = -1,	/* stop exploration with error */
  action_continue = 0,		/* continue exploration */
  action_skip_siblings,		/* skip the rest of the current directory */
  action_skip_subtree,		/* skip the subtree (valid when type==type_directory_pre) */
  action_stop_ok		/* stop exploration with success */
};

/* description of the entry */
struct fs_entry
{
  const char *base;		/* the given base directory explored */
  const char *path;		/* the path of the entry */
  const char *relpath;		/* the path of the entry relative to the base directory */
  const char *name;		/* the name of the entry within its directory */
  int length;			/* length of the string 'path' */
  enum fs_type type;		/* type of the entry */
  void *data;			/* user data */
};

int fs_explore (const char *directory, int wanted,
		enum fs_action (*callback) (const struct fs_entry * entry),
		void *data);
