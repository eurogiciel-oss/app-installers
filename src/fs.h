/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */
#pragma once

enum fs_type
{
  type_regular,
  type_directory_pre,
  type_directory_post,
  type_symlink,
  type_others
};

enum fs_wants
{
  wants_regular = 1,
  wants_directory_pre = 2,
  wants_directory_post = 4,
  wants_symlink = 8,
  wants_others = 16,
  wants_error_on_unwanted = 32,
  wants_any_pre =
    wants_regular | wants_symlink | wants_others | wants_directory_pre,
  wants_any_post =
    wants_regular | wants_symlink | wants_others | wants_directory_post
};

enum fs_action
{
  action_stop_error = -1,
  action_continue = 0,
  action_skip_siblings,
  action_skip_subtree,
  action_stop_ok
};

struct fs_entry
{
  const char *base;
  const char *path;
  const char *relpath;
  const char *name;
  int length;
  enum fs_type type;
  void *data;
};

typedef enum fs_action (*fs_callback) (const struct fs_entry * entry);


int fs_remove_directory_content (const char *path);
int fs_remove_directory (const char *path, int force);
int fs_remove_any (const char *path);

int fs_copy_file (const char *dest, const char *src, int force);
int fs_copy_directory (const char *dest, const char *src, int force);
