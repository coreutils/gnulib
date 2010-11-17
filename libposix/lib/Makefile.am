BUILT_SOURCES = lpx-config.h
SUFFIXES =

EXTRA_DIST = mk-lpx-config-h.sh
EXTRA_HEADERS =
nodist_pkginclude_HEADERS = lpx-config.h
nobase_nodist_pkginclude_HEADERS =
pkginclude_HEADERS = version.h

CLEANFILES =
MOSTLYCLEANDIRS =
MOSTLYCLEANFILES = lpx-config.h
MAINTAINERCLEANFILES =

include gnulib.mk

libposix_la_LDFLAGS += -version-info $(LTV_CURRENT):$(LTV_REVISION):$(LTV_AGE)
libposix_la_SOURCES += version.c

version.o : lpx-config.h
lpx-config.h : $(top_builddir)/config.h
	$(SHELL) $(srcdir)/mk-lpx-config-h.sh $(top_builddir)/config.h $@
