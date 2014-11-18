/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

#include <alloca.h>
#include <cassert>

#include "step.hxx"

namespace AppInstaller {

int Step::_process_(Step *step)
{
	try {
		return step->process();
	}
	catch (const FailException &e) {
		errno = e.saved_errno;
		return e.saved_retcode;
	}
	catch (...) {
		return -1;
	}
}

int Step::_undo_(Step *step)
{
	try {
		return step->undo();
	}
	catch (const FailException &e) {
		errno = e.saved_errno;
		return e.saved_retcode;
	}
	catch (...) {
		return -1;
	}
}

int Step::_clean_(Step *step)
{
	try {
		return step->clean();
	}
	catch (const FailException &e) {
		errno = e.saved_errno;
		return e.saved_retcode;
	}
	catch (...) {
		return -1;
	}
}

}


