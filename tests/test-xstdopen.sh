#!/bin/sh

# Test with all of stdin, stdout, stderr open.
./test-xstdopen || exit 1

# The syntax for closed file descriptors in sh scripts is specified by POSIX in
# section 2.7.5 of
# http://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html

# Test with stdin closed.
./test-xstdopen <&- || exit 1

# Test with stdout closed.
./test-xstdopen >&- || exit 1

# Test with stderr closed.
./test-xstdopen 2>&- || exit 1

# Test with all of stdin, stdout, stderr closed.
./test-xstdopen <&- >&- 2>&- || exit 1
