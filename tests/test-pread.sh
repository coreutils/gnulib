: ${srcdir=.} ${builddir=.}
. $srcdir/init.sh --set-path=$builddir

fail=0;
test-pread || fail=1

Exit $fail
