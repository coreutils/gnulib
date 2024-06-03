#!/bin/sh

# Test against a memory leak.

(${CHECKER} ./test-dprintf-posix2${EXEEXT} 0
 result=$?
 if test $result != 77 && test $result != 78 && test $result != 79 && test $result != 80 && test $result != 81; then result=1; fi
 exit $result
) 2>/dev/null
malloc_result=$?
if test $malloc_result = 77; then
  echo "Skipping test: no way to determine address space size"
  exit 77
fi
if test $malloc_result = 78; then
  echo "Skipping test: cannot trust address space size on this platform"
  exit 77
fi
if test $malloc_result = 79; then
  echo "Skipping test: cannot trust address space size when running under QEMU"
  exit 77
fi
if test $malloc_result = 80; then
  echo "Skipping test: address sanitizer's malloc behaves differently"
  exit 77
fi

${CHECKER} ./test-dprintf-posix2${EXEEXT} 1 > /dev/null
result=$?
if test $result = 77; then
  echo "Skipping test: no way to determine address space size"
  exit 77
fi
if test $result != 0; then
  exit 1
fi

if test $malloc_result = 81; then
  echo "Skipping test: get_rusage_as() doesn't work"
  exit 77
fi

exit 0
