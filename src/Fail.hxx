/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#pragma once

extern "C" {
#include "fail.h"
}

#include <errno.h>
#include <exception>

namespace AppInstaller {

class FailException : public std::exception {

  friend struct FailCode;

  /* constructor */
  inline FailException(int code) : saved_retcode(code) { saved_errno = errno; }

public:

  int saved_retcode;
  int saved_errno;

  /* Get the error string */
  const char *what();
};

struct FailCode {

  /* currently recorded code */
  int value;

  /* default constructor */
  inline FailCode(int code) : value(code) {
	if (should_throw && code < 0)
		throw FailException(code);
  }

  /* Convertion to integer */
  inline operator int() const { return value; }
  inline operator bool() const { return value >= 0; }

  /* global value stating if throw should be done on error (false by default) */
  static bool should_throw;
};

}

