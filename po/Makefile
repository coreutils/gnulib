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
	mv $(DOMAIN).po $(DOMAIN).pot

force:
