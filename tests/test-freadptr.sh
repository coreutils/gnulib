#!/bin/sh

./test-freadptr${EXEEXT} 5 < "$srcdir/test-freadptr.sh" || exit 1
exit 0
