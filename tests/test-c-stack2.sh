#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' 1 2 3 15

tmpfiles="t-c-stack2.tmp"

# Sanitize exit status within a subshell, since some shells fail to
# redirect stderr on their message about death due to signal.
(./test-c-stack${EXEEXT} 1; exit $?) 2> t-c-stack2.tmp

case $? in
  77) if grep 'stack overflow' t-c-stack2.tmp >/dev/null ; then
	echo 'cannot tell stack overflow from crash; consider installing libsigsegv' >&2
      else
       cat t-c-stack2.tmp >&2
      fi
      (exit 77); exit 77 ;;
  0) (exit 1); exit 1 ;;
esac
if grep 'program error' t-c-stack2.tmp >/dev/null ; then
  :
else
  (exit 1); exit 1
fi

rm -fr $tmpfiles

exit 0
