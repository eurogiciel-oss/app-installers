#/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

bin_PROGRAMS = wgt-installer

wgt_installer_CFLAGS = $(EXTDEPS_CFLAGS)

wgt_installer_LDADD = $(EXTDEPS_LIBS)

wgt_installer_SOURCES = \
	backend.c \
	fail.c \
	options.c \
	step.c \
	step-drm.c \
	step-unpack.c \
	step-check.c \
	step-manifest.c \
	utils.c
