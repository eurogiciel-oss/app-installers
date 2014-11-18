/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */
#pragma once

extern "C" {
#include "unzip.h"
}

#include <string>
#include "fail.hxx"

namespace AppInstaller {

namespace Utils {

inline ssize_t read (int fd, void *buf, size_t count) {
	return utils_read(fd, buf, count);
}

inline ssize_t write (int fd, void *buf, size_t count) {
	return utils_write(fd, buf, count);
}

}

}
