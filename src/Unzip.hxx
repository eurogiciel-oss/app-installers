/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */
#pragma once

extern "C" {
#include "unzip.h"
}

#include <string>
#include "fail.hxx"

namespace AppInstaller {

inline FailCode unzip(const std::string &targetdir, const std::string &zipfile) {
	return unzip (targetdir.c_str(), zipfile.c_str());
}

}
