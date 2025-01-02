all: gnulib.pot

include Makevars

VERSION != date -u +"%Y%m%d"

$(DOMAIN).pot: force
	xgettext --default-domain=$(DOMAIN) --directory=.. \
	  --add-comments=TRANSLATORS: \
	  --copyright-holder='$(COPYRIGHT_HOLDER)' \
	  --package-name='GNU gnulib' \
	  --package-version='$(VERSION)' \
	  --msgid-bugs-address='$(MSGID_BUGS_ADDRESS)' \
	  $(XGETTEXT_OPTIONS) \
	  `cd .. && find lib '(' -name '*.c' -o -name '*.h' -o -name '*.y' ')' | LC_ALL=C sort`
# We cannot use the usual comment
# "This file is distributed under the same license as the GNU gnulib package."
# here, because gnulib is not under a single license. Instead, use a comment
# "This file is distributed under the LGPLv2+ license."
# This comment is agreed-upon with the Translation Project.
	sed -e 's/^# This file is distributed under the same license.*/# This file is distributed under the LGPLv2+ license./' < $(DOMAIN).po > $(DOMAIN).po1
	mv $(DOMAIN).po1 $(DOMAIN).pot
	rm -f $(DOMAIN).po

force:
