#/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

bin_PROGRAMS = wgt-installer tpk-installer

commonsrcs = \
	fail.c \
	fs.c \
	options.c \
	step.c \
	step-check-signature.c \
	step-unpack.c \
	unzip.c \
	utils.c

wgt_installer_SOURCES = \
	$(commonsrcs) \
	backend-wgt.c \
	step-check-wgt.c 

wgt_installer_CFLAGS = $(EXTDEPS_CFLAGS)
wgt_installer_LDADD = $(EXTDEPS_LIBS)

tpk_installer_SOURCES = \
	$(commonsrcs) \
	backend-tpk.c \
	step-check-tpk.c 

tpk_installer_CFLAGS = $(EXTDEPS_CFLAGS)
tpk_installer_LDADD = $(EXTDEPS_LIBS)

