/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include "fs.hxx"

namespace AppInstaller {

Fs::Action Fs::exploreCallback(const Fs::Entry * entry)
{
	return (*reinterpret_cast<Fs::Explorer*>(entry->data))(*entry);
}

}

