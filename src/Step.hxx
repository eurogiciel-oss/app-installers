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

	inline operator const struct step *() const {
		asStep.process = (int(*)(void*)) _process_;
		asStep.undo = (int(*)(void*)) _undo_;
		asStep.clean = (int(*)(void*)) _clean_;
		asStep.data = (void*) this;
		return &asStep;
	}

private:

	mutable struct step asStep;
	static int _process_(Step *step);
	static int _undo_(Step *step);
	static int _clean_(Step *step);
};

}

