#!/bin/sh
. "${srcdir=.}/init.sh"

test "$SD_DLOPEN_SUPPORTED" = yes \
  || skip_ ".note.dlopen is not supported"
test "$OBJDUMP" != false \
  || skip_ "objdump is not available"

program="$initial_cwd_/test-sd-dlopen${EXEEXT}"
LC_ALL=C "$OBJDUMP" -h "$program" > sections \
  || skip_ "objdump cannot read the test executable"
grep '[.]note[.]dlopen' sections > /dev/null \
  || fail_ ".note.dlopen section is missing"

LC_ALL=C "$OBJDUMP" -s -j .note.dlopen "$program" > contents \
  || fail_ "cannot dump the .note.dlopen section"
vendor_count=$(grep -c '46444f00' contents)
test "$vendor_count" = 2 \
  || fail_ "expected two FDO dlopen notes, found $vendor_count"

Exit 0
