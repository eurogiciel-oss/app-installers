/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */
#pragma once

int utils_unzip (const char *targetdir, const char *zipfile);

int utils_remove_directory_content (const char *path);
int utils_remove_directory (const char *path, int force);
int utils_remove_any (const char *path);

int utils_copy_file (const char *dest, const char *src, int force);
int utils_copy_directory (const char *dest, const char *src, int force);
