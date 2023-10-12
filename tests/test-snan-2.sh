#!/bin/sh

# Test of signalling not-a-number.

final_rc=0

${CHECKER} ./test-snan-2${EXEEXT} f
rc=$?
if test $rc = 77; then
  final_rc=77
else
  if test $rc = 0; then
    echo "Use of SNaNf() did not signal." 1>&2
    exit 1
  fi
fi

${CHECKER} ./test-snan-2${EXEEXT} d
rc=$?
if test $rc = 77; then
  final_rc=77
else
  if test $rc = 0; then
    echo "Use of SNaNd() did not signal." 1>&2
    exit 1
  fi
fi

${CHECKER} ./test-snan-2${EXEEXT} l
rc=$?
if test $rc = 77; then
  final_rc=77
else
  if test $rc = 0; then
    echo "Use of SNaNl() did not signal." 1>&2
    exit 1
  fi
fi

exit $final_rc
