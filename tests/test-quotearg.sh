#!/bin/sh

# Choose an existing locale. The locale encoding does not matter; see the
# comment in test-quotearg.po.
if test $LOCALE_FR_UTF8 != none; then
  locale=$LOCALE_FR_UTF8
else
  if test $LOCALE_FR != none; then
    locale=$LOCALE_FR
  else
    locale=none
  fi
fi

LOCALE=$locale LOCALEDIR="$srcdir/locale" \
./test-quotearg${EXEEXT}
