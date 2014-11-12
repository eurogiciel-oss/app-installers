#/* 2014, Copyright © Intel Coporation, license MIT, see COPYING file */

bin_PROGRAMS = wgt-installer tpk-installer

noinst_LIBRARIES = libappinstallers.a

libappinstallers_a_SOURCES = \
	fail.c \
	fs.c \
	manifest-tpk.c \
	options.c \
	step.c \
	step-check-signature.c \
	step-check-tpk.c \
	step-check-wgt.c \
	step-manifest-tpk.c \
	step-manifest-wgt.c \
	step-unpack.c \
	unzip.c \
	utils.c \
	xml-read.c

wgt_installer_SOURCES = backend-wgt.c

wgt_installer_CFLAGS = $(EXTDEPS_CFLAGS)
wgt_installer_LDADD = libappinstallers.a $(EXTDEPS_LIBS)

tpk_installer_SOURCES = backend-tpk.c

tpk_installer_CFLAGS = $(EXTDEPS_CFLAGS)
tpk_installer_LDADD = libappinstallers.a $(EXTDEPS_LIBS)

