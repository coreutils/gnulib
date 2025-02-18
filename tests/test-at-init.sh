#!/bin/sh

output=`${CHECKER} ./test-at-init${EXEEXT}`
if test "$output" != 'Initializing...Main...338350...25502500...Finishing...'; then
  echo "Got output: $output" 1>&2
  exit 1
fi

exit 0
