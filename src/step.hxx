/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#pragma once

extern "C" {
#include "step.h"
}

#include <list>
#include <vector>

#include "fail.hxx"

namespace AppInstaller {

class Step {

private:

	virtual FailCode process() = 0;
	virtual FailCode undo() = 0;
	virtual FailCode clean() = 0;

public:

	inline operator struct step () const {
		struct step result;
		result.process = (int(*)(void*)) _process_;
		result.undo = (int(*)(void*)) _undo_;
		result.clean = (int(*)(void*)) _clean_;
		result.data = (void*) this;
		return result;
	}

	static FailCode run(const std::vector<Step*> &vector);

private:
	static int _process_(Step *step);
	static int _undo_(Step *step);
	static int _clean_(Step *step);
};

}

