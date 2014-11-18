/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#pragma once

extern "C" {
#include <sys/types.h>
#include "fs.h"
}

#include <string>
#include "fail.hxx"

namespace AppInstaller {

struct Fs {

	/* Removes any file in the directory of 'path' */
	static inline FailCode removeDirectoryContent(const std::string &path) {
		return fs_remove_directory_content (path.c_str());
	}

	/* Removes the directory of 'path'. If 'force'==0 the operation
	   fails if the directory has an other content than "." and "..".
	   If 'force'!=0 then the content of the directory is removed
	   before. */
	static inline FailCode removeDirectory(const std::string &path, bool force) {
		return fs_remove_directory (path.c_str(), force);
	}

	/* Removes any file or directory of 'path'. If 'path' is a
	   directory, its content is removed. */
	static inline FailCode removeAny(const std::string &path) {
		return fs_remove_any (path.c_str());
	}

	/* Copies the file of path 'src' in the file of path 'dest'.
	   If 'force'==0 no copy is done if 'dest' already exists.
	   If 'force'!=0 the file 'dest' is overwriten if it exists. */
	static inline FailCode copyFile (const std::string &dest, const std::string &src, bool force) {
		return fs_copy_file (dest.c_str(), src.c_str(), force);
	}

	/* Copies recursively the content of the directory of path 'src'
	   to the directory of path 'dest' ('dest' is created if it doesn't exist).
	   If 'force'==0 existing files are not overwriten and an error if returned.
	   If 'force'==1 existing files are overwriten. SEE: fs_set_mkdir_mode. */
	static inline FailCode copyDirectory (const std::string &dest, const std::string &src, bool force) {
		return fs_copy_directory(dest.c_str(), src.c_str(), force);
	}

	/* Create the directory of 'path' and if needed its parents
	   with the current creation mode. Don't returns error if the
	   directory already exists. SEE: fs_set_mkdir_mode. */
	static inline FailCode mkdir (const std::string &path) {
		return fs_mkdir(path.c_str());
	}

	/* Sete the default mode for creating directories.
	   Returns the previous one. CAUTION: not thread safe. */
	static inline mode_t setMkdirMode(mode_t mode) {
		return fs_set_mkdir_mode (mode);
	}

	typedef enum fs_type Type;
	typedef enum fs_wants Wants;
	typedef enum fs_action Action;
	typedef struct fs_entry Entry;

	struct Explorer {
		virtual Action operator()(const Entry &entry) = 0;
	};

	static inline FailCode explore(const std::string &directory, Wants wanted, Explorer &explorer) {
		return fs_explore(directory.c_str(), wanted, exploreCallback, &explorer);
	}

private:

	static Action exploreCallback(const Entry * entry);
};

}

