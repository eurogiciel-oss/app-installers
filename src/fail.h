/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#pragma once

#include <stdarg.h>

extern int vfail (int err, const char *format, va_list list);
extern int fail (int err, const char *format, ...);
extern int fail_out_of_memory ();
