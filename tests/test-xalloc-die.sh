#!/bin/sh

tmpfiles=""
trap '__st=$?; rm -fr $tmpfiles; exit $__st' 0
trap '__st=$?; (exit $__st); exit $__st' 1 2 3 15

if ( diff --version < /dev/null 2>&1 | grep GNU ) 2>&1 > /dev/null; then
  compare() { diff -u "$@"; }
elif ( cmp --version < /dev/null 2>&1 | grep GNU ) 2>&1 > /dev/null; then
  compare() { cmp -s "$@"; }
else
  compare() { cmp "$@"; }
fi

tmpfiles="t-xalloc-die.tmp"
PATH=".:$PATH"
export PATH
test-xalloc-die${EXEEXT} 2> t-xalloc-die.tmp
case $? in
  1) ;;
  *) (exit 1); exit 1 ;;
esac

compare - t-xalloc-die.tmp <<\EOF || { (exit 1); exit 1; }
test-xalloc-die: memory exhausted
EOF

rm -fr $tmpfiles

exit 0
