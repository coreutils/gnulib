#!/bin/sh
. "${srcdir=.}/init.sh"

# Rather than figure out how to invoke the compiler with the right
# include path ourselves, we let make do it:
(cd "$initial_cwd_" && rm -f test-verify.o \
    && $MAKE test-verify.o >/dev/null 2>&1) \
  || skip_ "cannot compile error-free"

# Now, prove that we encounter all expected compilation failures:
: >out
: >err
for i in 1 2 3 4 5; do
  (cd "$initial_cwd_"
   rm -f test-verify.o
   $MAKE CFLAGS=-DEXP_FAIL=$i test-verify.o) >>out 2>>err \
  && { warn_ "compiler didn't detect verification failure $i"; fail=1; }
done

Exit $fail
